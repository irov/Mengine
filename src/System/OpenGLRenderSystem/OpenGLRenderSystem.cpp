#include "OpenGLRenderSystem.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderError.h"

#include "Interface/StringizeInterface.h"
#include "Interface/PlatformInterface.h"

#include "OpenGLRenderExtension.h"

#include "Factory/FactoryDefault.h"
#include "Factory/FactoryPool.h"
#include "Factory/FactoryPoolWithListener.h"

#include "Logger/Logger.h"

#include <cmath>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
#define GLUNUSED(x) ((void)(x))
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::OpenGLRenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        static constexpr float get_A_float_from_argb32( uint32_t argb )
        {
            float a = (argb >> 24) / 255.0f;

            return a;
        }
        static constexpr float get_R_float_from_argb32( uint32_t argb )
        {
            float r = ((argb >> 16) & 0xFF) / 255.0f;

            return r;
        }
        static constexpr float get_G_float_from_argb32( uint32_t argb )
        {
            float g = ((argb >> 8) & 0xFF) / 255.0f;

            return g;
        }
        static constexpr float get_B_float_from_argb32( uint32_t argb )
        {
            float b = (argb & 0xFF) / 255.0f;

            return b;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderSystem::OpenGLRenderSystem()
        : m_glMaxClipPlanes( 0 )
        , m_glMaxCombinedTextureImageUnits( 0 )
        , m_renderWindowCreate( false )
        , m_depthMask( false )
    {
        mt::ident_m4( m_worldMatrix );
        mt::ident_m4( m_viewMatrix );
        mt::ident_m4( m_projectionMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderSystem::~OpenGLRenderSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::_initialize()
    {
        LOGGER_WARNING( "Initializing OpenGL RenderSystem..." );

#ifndef MENGINE_OPENGL_ES
        m_renderPlatform = STRINGIZE_STRING_LOCAL( "OpenGL" );
#else
        m_renderPlatform = STRINGIZE_STRING_LOCAL( "OpenGLES" );
#endif

        m_factoryVertexBuffer = new FactoryDefault<OpenGLRenderVertexBuffer>();
        m_factoryIndexBuffer = new FactoryDefault<OpenGLRenderIndexBuffer>();
        m_factoryRenderImage = Helper::makeFactoryPoolWithListener<OpenGLRenderImage, 128>( this, &OpenGLRenderSystem::onRenderImageDestroy_ );
        m_factoryRenderVertexAttribute = new FactoryPool<OpenGLRenderVertexAttribute, 16>();
        m_factoryRenderFragmentShader = Helper::makeFactoryPoolWithListener<OpenGLRenderFragmentShader, 16>( this, &OpenGLRenderSystem::onRenderFragmentShaderDestroy_ );
        m_factoryRenderVertexShader = Helper::makeFactoryPoolWithListener<OpenGLRenderVertexShader, 16>( this, &OpenGLRenderSystem::onRenderVertexShaderDestroy_ );
        m_factoryProgram = Helper::makeFactoryPoolWithListener<OpenGLRenderProgram, 16>( this, &OpenGLRenderSystem::onRenderProgramDestroy_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::_finalize()
    {
        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            TextureStage & stage = m_textureStage[i];

            if( stage.texture != nullptr )
            {
                stdex::intrusive_ptr_release( stage.texture );
                stage.texture = nullptr;
            }
        }

        m_currentProgram = nullptr;

        m_currentIndexBuffer = nullptr;
        m_currentVertexBuffer = nullptr;

        m_deferredCompileVertexShaders.clear();
        m_deferredCompileFragmentShaders.clear();
        m_deferredCompilePrograms.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryVertexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryIndexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexAttribute );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderFragmentShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryProgram );

        m_factoryVertexBuffer = nullptr;
        m_factoryIndexBuffer = nullptr;
        m_factoryRenderImage = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryProgram = nullptr;

        m_cacheRenderImages.clear();
        m_cacheRenderVertexShaders.clear();
        m_cacheRenderFragmentShaders.clear();
        m_cacheRenderPrograms.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderPlatform OpenGLRenderSystem::getRenderPlatformType() const
    {
#ifndef MENGINE_OPENGL_ES
        return RP_OPENGL;
#else
        return RP_OPENGLES;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & OpenGLRenderSystem::getRenderPlatformName() const
    {
        return m_renderPlatform;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount )
    {
        GLUNUSED( _bits );
        GLUNUSED( _fullscreen );
        GLUNUSED( _waitForVSync );
        GLUNUSED( _FSAAType );
        GLUNUSED( _FSAAQuality );
        GLUNUSED( _MultiSampleCount );

        m_resolution = _resolution;

#ifndef MENGINE_OPENGL_ES
        Mengine::initialize_GLEXT();
#endif

        const char* vendorStr = reinterpret_cast<const char*>(glGetString( GL_VENDOR ));
        const char* rendererStr = reinterpret_cast<const char*>(glGetString( GL_RENDERER ));
        const char* versionStr = reinterpret_cast<const char*>(glGetString( GL_VERSION ));
        const char* extensionsStr = reinterpret_cast<const char*>(glGetString( GL_EXTENSIONS ));

        LOGGER_WARNING( "Vendor      : %s", vendorStr );
        LOGGER_WARNING( "Renderer    : %s", rendererStr );
        LOGGER_WARNING( "Version     : %s", versionStr );
        LOGGER_WARNING( "Extensions  : %s", extensionsStr );

        OPENGL_RENDER_CHECK_ERROR();

        //        GLint maxClipPlanes;
        //        glGetIntegerv( GL_MAX_CLIP_PLANES, &maxClipPlanes );

        m_glMaxClipPlanes = 0;

        GLint maxCombinedTextureImageUnits;
        glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits );

        m_glMaxCombinedTextureImageUnits = maxCombinedTextureImageUnits;

        for( uint32_t i = 0; i < MENGINE_MAX_TEXTURE_STAGES; ++i )
        {            
            m_textureStage[i] = TextureStage();
        }

        GLCALL( glFrontFace, (GL_CW) );
        GLCALL( glEnable, (GL_DEPTH_TEST) );
        GLCALL( glDisable, (GL_STENCIL_TEST) );
        GLCALL( glDisable, (GL_CULL_FACE) );
        //        GLCALL( glDisable, (GL_LIGHTING) );
        GLCALL( glDisable, (GL_BLEND) );
        //        GLCALL( glDisable, (GL_ALPHA_TEST) );
        GLCALL( glDisable, (GL_DITHER) );

        GLCALL( glDepthMask, (GL_FALSE) );
        GLCALL( glDepthFunc, (GL_LESS) );

#ifndef MENGINE_OPENGL_ES
        GLCALL( glMatrixMode, (GL_MODELVIEW) );
        GLCALL( glLoadIdentity, () );

        GLCALL( glMatrixMode, (GL_PROJECTION) );
        GLCALL( glLoadIdentity, () );

        GLCALL( glMatrixMode, (GL_TEXTURE) );
        GLCALL( glLoadIdentity, () );
#endif

        for( TVectorRenderVertexShaders::const_iterator 
            it = m_deferredCompileVertexShaders.begin(),
            it_end = m_deferredCompileVertexShaders.end();
            it != it_end;
            ++it )
        {
            const OpenGLRenderVertexShaderPtr & shader = *it;

            if( shader->compile() == false )
            {
                return false;
            }
        }

        m_deferredCompileVertexShaders.clear();

        for( TVectorRenderFragmentShaders::const_iterator
            it = m_deferredCompileFragmentShaders.begin(),
            it_end = m_deferredCompileFragmentShaders.end();
            it != it_end;
            ++it )
        {
            const OpenGLRenderFragmentShaderPtr & shader = *it;

            if( shader->compile() == false )
            {
                return false;
            }
        }

        m_deferredCompileFragmentShaders.clear();

        for( TVectorDeferredRenderPrograms::const_iterator
            it = m_deferredCompilePrograms.begin(),
            it_end = m_deferredCompilePrograms.end();
            it != it_end;
            ++it )
        {
            const OpenGLRenderProgramPtr & program = *it;

            if( program->compile() == false )
            {
                return false;
            }
        }

        m_deferredCompilePrograms.clear();

        m_renderWindowCreate = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect )
    {
        GLUNUSED( _image );
        GLUNUSED( _rect );
        //ToDo!

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setClipplaneCount( uint32_t _count )
    {
        GLUNUSED( _count );
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setClipplane( uint32_t _i, const mt::planef & _plane )
    {
        GLUNUSED( _i );
        GLUNUSED( _plane );
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setViewport( const Viewport & _viewport )
    {
        const GLsizei xb = static_cast<GLsizei>(_viewport.begin.x);
        const GLsizei yb = static_cast<GLsizei>(_viewport.end.y);
        const GLsizei w = static_cast<GLsizei>(_viewport.getWidth());
        const GLsizei h = static_cast<GLsizei>(_viewport.getHeight());

        const GLsizei height = static_cast<GLsizei>(m_resolution.getHeight());

        GLCALL( glViewport, (xb, height - yb, w, h) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setViewMatrix( const mt::mat4f & _view )
    {
        m_viewMatrix = _view;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
    {
        m_projectionMatrix = _projection;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture )
    {
        GLUNUSED( _stage );
        GLUNUSED( _texture );
        // To Do
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setWorldMatrix( const mt::mat4f & _world )
    {
        m_worldMatrix = _world;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr OpenGLRenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType )
    {
        OpenGLRenderVertexBufferPtr buffer = m_factoryVertexBuffer->createObject();

        if( buffer->initialize( _vertexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        m_currentVertexBuffer = _vertexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr OpenGLRenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType )
    {
        OpenGLRenderIndexBufferPtr buffer = m_factoryIndexBuffer->createObject();

        if( buffer->initialize( _indexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        m_currentIndexBuffer = _indexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr OpenGLRenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize )
    {
        OpenGLRenderVertexAttributePtr vertexAttribute = m_factoryRenderVertexAttribute->createObject();

        if( vertexAttribute == nullptr )
        {
            LOGGER_ERROR( "invalid create vertex attribute '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( vertexAttribute->initialize( _name, _elementSize ) == false )
        {
            LOGGER_ERROR( "invalid initialize vertex attribute '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr OpenGLRenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        (void)_compile;

        OpenGLRenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject();

        if( shader == nullptr )
        {
            LOGGER_ERROR( "invalid create shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( shader->initialize( _name, _memory ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( m_renderWindowCreate == true )
        {
            if( shader->compile() == false )
            {
                LOGGER_ERROR( "invalid compile shader '%s'"
                    , _name.c_str()
                );

                return nullptr;
            }
        }
        else
        {
            m_deferredCompileFragmentShaders.push_back( shader );
        }

        OpenGLRenderFragmentShader * shader_ptr = shader.get();
        m_cacheRenderFragmentShaders.push_back( shader_ptr );

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr OpenGLRenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        (void)_compile;

        OpenGLRenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject();

        if( shader == nullptr )
        {
            LOGGER_ERROR( "invalid create shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( shader->initialize( _name, _memory ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( m_renderWindowCreate == true )
        {
            if( shader->compile() == false )
            {
                LOGGER_ERROR( "invalid compile shader '%s'"
                    , _name.c_str()
                );

                return nullptr;
            }
        }
        else
        {
            m_deferredCompileVertexShaders.push_back( shader );
        }

        OpenGLRenderVertexShader * shader_ptr = shader.get();
        m_cacheRenderVertexShaders.push_back( shader_ptr );

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr OpenGLRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount )
    {
        OpenGLRenderProgramPtr program = m_factoryProgram->createObject();

        if( program == nullptr )
        {
            LOGGER_ERROR( "invalid create program '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( program->initialize( _name, _vertex, _fragment, _vertexAttribute, _samplerCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( m_renderWindowCreate == true )
        {
            if( program->compile() == false )
            {
                LOGGER_ERROR( "invalid compile program '%s'"
                    , _name.c_str()
                );

                return nullptr;
            }
        }
        else
        {
            m_deferredCompilePrograms.push_back( program );
        }

        OpenGLRenderProgram * program_ptr = program.get();
        m_cacheRenderPrograms.push_back( program_ptr );

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        m_currentProgram = stdex::intrusive_static_cast<OpenGLRenderProgramPtr>(_program);
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        GLUNUSED( _program );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::drawIndexedPrimitive( EPrimitiveType _type,
        uint32_t _baseVertexIndex, uint32_t _minIndex,
        uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
    {
        GLUNUSED( _verticesNum );
        GLUNUSED( _minIndex );
        GLUNUSED( _baseVertexIndex );

        if( m_currentIndexBuffer == nullptr || m_currentVertexBuffer == nullptr || m_currentProgram == nullptr )
        {
            return;
        }

        if( m_currentProgram->enable() == false )
        {
            return;
        }

        m_currentProgram->bindMatrix( m_worldMatrix, m_viewMatrix, m_projectionMatrix );

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            const TextureStage & textureStage = m_textureStage[i];
#ifdef MENGINE_OPENGL_ES
            GLCALL( glActiveTexture, (GL_TEXTURE0 + i) );
#else
            GLCALL( glActiveTexture_, (GL_TEXTURE0 + i) );
#endif
            if( textureStage.texture == nullptr )
            {
                GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );

                continue;
            }

            GLuint texture_uid = textureStage.texture->getUId();

            GLCALL( glBindTexture, (GL_TEXTURE_2D, texture_uid) );

            if( m_currentProgram->bindTexture( i ) == false )
            {
                continue;
            }

            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter) );
        }

        if( m_currentIndexBuffer->enable() == false )
        {
            return;
        }

        if( m_currentVertexBuffer->enable() == false )
        {
            return;
        }

        const RenderVertexAttributeInterfacePtr & vertexAttribute = m_currentProgram->getVertexAttribute();

        if( vertexAttribute->enable() == false )
        {
            return;
        }

        GLenum mode = s_getGLPrimitiveMode( _type );
        const RenderIndex * baseIndex = nullptr;
        const RenderIndex * offsetIndex = baseIndex + _startIndex;

        GLenum indexType = s_getGLIndexType( sizeof( RenderIndex ) );

        GLCALL( glDrawElements, (mode, _indexCount, indexType, reinterpret_cast<const GLvoid *>(offsetIndex)) );

        vertexAttribute->disable();

        m_currentIndexBuffer->disable();
        m_currentVertexBuffer->disable();

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            TextureStage & textureStage = m_textureStage[i];

            if( textureStage.texture == nullptr )
            {
                break;
            }
#ifdef MENGINE_OPENGL_ES
            GLCALL( glActiveTexture, (GL_TEXTURE0 + i) );
#else
            GLCALL( glActiveTexture_, (GL_TEXTURE0 + i) );
#endif

            GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );
        }

        m_currentProgram->disable();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        TextureStage & tStage = m_textureStage[_stage];

        stdex::intrusive_ptr_release( tStage.texture );

        if( _texture != nullptr )
        {
            OpenGLRenderImage * texture = stdex::intrusive_get<OpenGLRenderImage *>( _texture );
            stdex::intrusive_ptr_setup( tStage.texture, texture );
        }
        else
        {
            tStage.texture = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        GLenum modeUGL = s_getGLAddressMode( _modeU );
        GLenum modeVGL = s_getGLAddressMode( _modeV );

        m_textureStage[_stage].wrapS = modeUGL;
        m_textureStage[_stage].wrapT = modeVGL;
        m_textureStage[_stage].border = _border;
    }
    //////////////////////////////////////////L////////////////////////////////
    void OpenGLRenderSystem::setTextureFactor( uint32_t _color )
    {
        GLUNUSED( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op )
    {
        const GLenum srcBlendFactor = s_toGLBlendFactor( _src );
        const GLenum dstBlendFactor = s_toGLBlendFactor( _dst );
        const GLenum blendOp = s_toGLBlendFactor( _op );

        GLCALL( glBlendFunc, (srcBlendFactor, dstBlendFactor) );

#ifdef MENGINE_OPENGL_ES
        GLCALL( glBlendEquation, (blendOp) );
#else
        GLCALL( glBlendEquation_, (blendOp) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setCullMode( ECullMode _mode )
    {
        bool enable = true;
        GLenum face = GL_BACK;

        if( _mode == CM_CULL_CW )
        {
            face = GL_FRONT;
        }
        else if( _mode == CM_CULL_NONE )
        {
            enable = false;
        }

        GLCALL( glCullFace, (face) );

        if( enable == true )
        {
            GLCALL( glEnable, (GL_CULL_FACE) );
        }
        else
        {
            GLCALL( glDisable, (GL_CULL_FACE) );
        }
    }
    /////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setSeparateAlphaBlendMode()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    size_t OpenGLRenderSystem::getTextureMemoryUse() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderSystem::getTextureCount() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        if( _depthTest == true )
        {
            GLCALL( glEnable, (GL_DEPTH_TEST) );
        }
        else
        {
            GLCALL( glDisable, (GL_DEPTH_TEST) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        m_depthMask = _depthWrite;

        GLCALL( glDepthMask, (m_depthMask ? GL_TRUE : GL_FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        const GLenum cmpFunc = s_toGLCmpFunc( _depthFunction );
        GLCALL( glDepthFunc, (cmpFunc) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setFillMode( EFillMode _mode )
    {
#ifndef MENGINE_OPENGL_ES
        const GLenum mode = s_getGLFillMode( _mode );
        glPolygonMode( GL_FRONT_AND_BACK, mode );
#endif      
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        GLCALL( glColorMask, (_r ? GL_TRUE : GL_FALSE, _g ? GL_TRUE : GL_FALSE, _b ? GL_TRUE : GL_FALSE, _a ? GL_TRUE : GL_FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        if( _alphaBlend == true )
        {
            GLCALL( glEnable, (GL_BLEND) );
        }
        else
        {
            GLCALL( glDisable, (GL_BLEND) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        TextureStage & tStage = m_textureStage[_stage];

        tStage.minFilter = s_toGLMinFilter( _minification, _mipmap );
        tStage.magFilter = s_toMagFilter( _magnification );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::findFormatFromChannels_( PixelFormat _format, uint32_t _channels, PixelFormat & _hwFormat, uint32_t & _hwChannels ) const
    {
        switch( _format )
        {
        case PF_UNKNOWN:
            {
                if( _channels == 1 )
                {
                    _hwFormat = PF_A8;
                    _hwChannels = 1;
                }
                else if( _channels == 3 )
                {
                    _hwFormat = PF_X8R8G8B8;    //original
                    //_hwFormat = PF_X8B8G8R8;
                    _hwChannels = 4;            // original
                }
                else if( _channels == 4 )
                {
                    _hwFormat = PF_A8R8G8B8; //original
                    _hwChannels = 4;
                }
            }break;
        default:
            {
                _hwFormat = _format;
                _hwChannels = _channels;
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
    {
        GLUNUSED( _depth );

        uint32_t hwChannels = 0;
        PixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

        GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

        if( textureInternalFormat == 0 )
        {
            LOGGER_ERROR( "invalid get GL Texture Internal format for PF %d"
                , hwFormat
            );

            return nullptr;
        }

        GLint textureColorFormat = s_toGLColorFormat( hwFormat );

        if( textureColorFormat == 0 )
        {
            LOGGER_ERROR( "invalid get GL Texture Color format for PF %d"
                , hwFormat
            );

            return nullptr;
        }

        GLint textureColorDataType = s_getGLColorDataType( hwFormat );

        if( textureColorDataType == 0 )
        {
            LOGGER_ERROR( "invalid get GL Color Data Type for PF %d"
                , hwFormat
            );

            return nullptr;
        }

        OpenGLRenderImagePtr image = m_factoryRenderImage->createObject();

        if( image == nullptr )
        {
            LOGGER_ERROR( "invalid create"
            );

            return nullptr;
        }

        if( image->initialize( ERIM_NORMAL
            , _mipmaps
            , _width
            , _height
            , hwChannels
            , hwFormat
            , textureInternalFormat
            , textureColorFormat
            , textureColorDataType ) == false )
        {
            LOGGER_ERROR( "invalid initialize"
            );

            return nullptr;
        }

        OpenGLRenderImage * image_ptr = image.get();
        m_cacheRenderImages.push_back( image_ptr );

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::lockRenderTarget( const RenderImageInterfacePtr & _renderTarget )
    {
        GLUNUSED( _renderTarget );
        //TODO
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::unlockRenderTarget()
    {
        //TODO
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::beginScene()
    {
        this->clearFrameBuffer( FBT_COLOR | FBT_DEPTH, 0x00000000, 1.f, 0x00000000 );

        m_currentIndexBuffer = nullptr;
        m_currentVertexBuffer = nullptr;

        m_currentProgram = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::endScene()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::swapBuffers()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
    {
        GLbitfield frameBufferFlags = 0;

        if( (_frameBufferTypes & FBT_COLOR) != 0 )
        {
            frameBufferFlags |= GL_COLOR_BUFFER_BIT;

            GLCALL( glClearColor, (
                Detail::get_R_float_from_argb32( _color ),
                Detail::get_G_float_from_argb32( _color ),
                Detail::get_B_float_from_argb32( _color ),
                Detail::get_A_float_from_argb32( _color ) )
            );
        }

        if( (_frameBufferTypes & FBT_DEPTH) != 0 )
        {
            frameBufferFlags |= GL_DEPTH_BUFFER_BIT;

            if( m_depthMask == false )
            {
                GLCALL( glDepthMask, (GL_TRUE) );
            }

#ifndef MENGINE_OPENGL_ES
            GLCALL( glClearDepth, (static_cast<GLclampd>(_depth)) );
#endif          
        }

        if( (_frameBufferTypes & FBT_STENCIL) != 0 )
        {
            frameBufferFlags |= GL_STENCIL_BUFFER_BIT;

            GLCALL( glClearStencil, (_stencil) );
        }

        GLCALL( glClear, (frameBufferFlags) );

        if( m_depthMask == false )
        {
            GLCALL( glDepthMask, (GL_FALSE) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        GLUNUSED( _fullscreen );

        m_resolution = _resolution;

        //m_windowContext->setFullscreenMode( _resolution.getWidth(), _resolution.getHeight(), _fullscreen );
        //glViewport( 0, 0, _resolution.getWidth(), _resolution.getHeight() );
        //glViewport( 0, 0, _resolution.getHeight(), _resolution.getWidth() );
        //glScissor( 0, 0, _resolution.getWidth(), _resolution.getHeight() );
        //m_winWidth = _resolution.getWidth();
        //m_winHeight = _resolution.getHeight();
        //m_winContextWidth = _resolution.getWidth();
        //m_winContextHeight = _resolution.getHeight();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::supportTextureFormat( PixelFormat _format ) const
    {
        return s_toGLInternalFormat( _format ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::supportTextureNonPow2() const
    {
        //return true;
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderSystem::getMaxCombinedTextureImageUnits() const
    {
        return m_glMaxCombinedTextureImageUnits;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onWindowMovedOrResized()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onWindowClose()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        (void)_fullscreen;

        for( TVectorCacheRenderImages::iterator
            it = m_cacheRenderImages.begin(),
            it_end = m_cacheRenderImages.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderImage * image = *it;

            image->release();
        }

        for( TVectorCacheRenderVertexShaders::iterator
            it = m_cacheRenderVertexShaders.begin(),
            it_end = m_cacheRenderVertexShaders.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderVertexShader * shader = *it;

            shader->release();
        }

        for( TVectorCacheRenderFragmentShaders::iterator
            it = m_cacheRenderFragmentShaders.begin(),
            it_end = m_cacheRenderFragmentShaders.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderFragmentShader * shader = *it;

            shader->release();
        }

        for( TVectorCacheRenderPrograms::iterator
            it = m_cacheRenderPrograms.begin(),
            it_end = m_cacheRenderPrograms.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderProgram * program = *it;

            program->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        (void)_fullscreen;

        for( TVectorCacheRenderImages::iterator
            it = m_cacheRenderImages.begin(),
            it_end = m_cacheRenderImages.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderImage * image = *it;

            image->reload();
        }

        for( TVectorCacheRenderVertexShaders::iterator
            it = m_cacheRenderVertexShaders.begin(),
            it_end = m_cacheRenderVertexShaders.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderVertexShader * shader = *it;

            shader->compile();
        }

        for( TVectorCacheRenderFragmentShaders::iterator
            it = m_cacheRenderFragmentShaders.begin(),
            it_end = m_cacheRenderFragmentShaders.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderFragmentShader * shader = *it;

            shader->compile();
        }

        for( TVectorCacheRenderPrograms::iterator
            it = m_cacheRenderPrograms.begin(),
            it_end = m_cacheRenderPrograms.end();
            it != it_end;
            ++it )
        {
            OpenGLRenderProgram * program = *it;

            program->compile();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setVSync( bool _vSync )
    {
        GLUNUSED( _vSync );
        //m_windowContext->setVSync( _vSync );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::clear( uint32_t _color, bool _force )
    {
        GLUNUSED( _color );
        GLUNUSED( _force );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
    {
        GLUNUSED( _depth );

        uint32_t hwChannels = 0;
        PixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

        GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

        if( textureInternalFormat == 0 )
        {
            LOGGER_ERROR( "invalid get GL Texture Internal format for PF %d"
                , hwFormat
            );

            return nullptr;
        }

        GLint textureColorFormat = s_toGLColorFormat( hwFormat );

        if( textureColorFormat == 0 )
        {
            LOGGER_ERROR( "invalid get GL Texture Color format for PF %d"
                , hwFormat
            );

            return nullptr;
        }

        GLint textureColorDataType = s_getGLColorDataType( hwFormat );

        if( textureColorDataType == 0 )
        {
            LOGGER_ERROR( "invalid get GL Color Data Type for PF %d"
                , hwFormat
            );

            return nullptr;
        }

        OpenGLRenderImagePtr texture = m_factoryRenderImage->createObject();

        if( texture->initialize( ERIM_DYNAMIC
            , 1
            , _width
            , _height
            , hwChannels
            , hwFormat
            , textureInternalFormat
            , textureColorFormat
            , textureColorDataType ) == false )
        {
            LOGGER_ERROR( "invalid initialize"
            );

            return nullptr;
        }

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr OpenGLRenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, PixelFormat _format )
    {
        GLUNUSED( _width );
        GLUNUSED( _height );
        GLUNUSED( _format );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr OpenGLRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format )
    {
        GLUNUSED( _width );
        GLUNUSED( _height );
        GLUNUSED( _format );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createRenderTargetImage( const RenderTargetInterfacePtr & _renderTarget )
    {
        GLUNUSED( _renderTarget );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
    {
        mt::mat4f scale;
        mt::make_scale_m4( scale, 1.f, 1.f, 1.f );

        mt::mat4f translation;
        mt::make_translation_m4( translation, 0.f, 0.f, 0.f );

        mt::mat4f transform;
        mt::mul_m4_m4( transform, scale, translation );

        mt::mat4f ortho;
        mt::make_projection_ortho_lh_m4( ortho, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

        mt::mul_m4_m4( _projectionMatrix, transform, ortho );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float _zn, float _zf )
    {
        mt::mat4f scale;
        mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

        mt::mat4f translation;
        mt::make_translation_m4( translation, -0.5f, +0.5f, 0.0f );

        mt::mat4f transform;
        mt::mul_m4_m4( transform, scale, translation );

        mt::mat4f projection_fov;
        mt::make_projection_fov_m4( projection_fov, _fov, _aspect, _zn, _zf );

        mt::mul_m4_m4( _projectionMatrix, transform, projection_fov );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
    {
        mt::mat4f scale;
        mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

        mt::mat4f translation;
        mt::make_translation_m4( translation, -0.5f, -0.5f, 0.0f );

        mt::mat4f transform;
        mt::mul_m4_m4( transform, scale, translation );

        mt::mat4f frustum;
        mt::make_projection_frustum_m4( frustum, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

        mt::mul_m4_m4( _projectionMatrix, transform, frustum );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport )
    {
        GLUNUSED( _viewport );

        mt::mat4f wm;
        mt::ident_m4( wm );

        mt::inv_m4_m4( _viewMatrix, wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
    {
        mt::make_lookat_m4( _viewMatrix, _eye, _dir, _up, _sign );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderImageDestroy_( OpenGLRenderImage * _image )
    {
        TVectorCacheRenderImages::iterator it_found = std::find( m_cacheRenderImages.begin(), m_cacheRenderImages.end(), _image );

        if( it_found == m_cacheRenderImages.end() )
        {
            return;
        }

        OpenGLRenderImage * image = *it_found;
        image->finalize();

        *it_found = m_cacheRenderImages.back();
        m_cacheRenderImages.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderVertexShaderDestroy_( OpenGLRenderVertexShader * _vertexShader )
    {
        TVectorCacheRenderVertexShaders::iterator it_found = std::find( m_cacheRenderVertexShaders.begin(), m_cacheRenderVertexShaders.end(), _vertexShader );

        if( it_found == m_cacheRenderVertexShaders.end() )
        {
            return;
        }

        *it_found = m_cacheRenderVertexShaders.back();
        m_cacheRenderVertexShaders.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderFragmentShaderDestroy_( OpenGLRenderFragmentShader * _fragmentShader )
    {
        TVectorRenderFragmentShaders::iterator it_found = std::find( m_deferredCompileFragmentShaders.begin(), m_deferredCompileFragmentShaders.end(), _fragmentShader );

        if( it_found == m_deferredCompileFragmentShaders.end() )
        {
            return;
        }

        *it_found = m_deferredCompileFragmentShaders.back();
        m_deferredCompileFragmentShaders.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderProgramDestroy_( OpenGLRenderProgram * _program )
    {
        TVectorCacheRenderPrograms::iterator it_found = std::find( m_cacheRenderPrograms.begin(), m_cacheRenderPrograms.end(), _program );

        if( it_found == m_cacheRenderPrograms.end() )
        {
            return;
        }

        *it_found = m_cacheRenderPrograms.back();
        m_cacheRenderPrograms.pop_back();
    }
}
