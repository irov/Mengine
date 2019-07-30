#include "OpenGLRenderSystem.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderError.h"

#include "Interface/PlatformInterface.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/ConstStringHelper.h"

#include "Kernel/Logger.h"

#include <cmath>
#include <algorithm>

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
        mt::ident_m4( m_totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderSystem::~OpenGLRenderSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::_initializeService()
    {
        LOGGER_WARNING( "Initializing OpenGL RenderSystem..." );

#ifndef MENGINE_RENDER_OPENGL_ES
        m_renderPlatform = STRINGIZE_STRING_LOCAL( "OpenGL" );
#else
        m_renderPlatform = STRINGIZE_STRING_LOCAL( "OpenGLES" );
#endif

        m_factoryRenderVertexBuffer = new FactoryDefault<OpenGLRenderVertexBuffer>();
        m_factoryRenderIndexBuffer = new FactoryDefault<OpenGLRenderIndexBuffer>();
        m_factoryRenderImage = Helper::makeFactoryPoolWithListener<OpenGLRenderImage, 128>( this, &OpenGLRenderSystem::onRenderImageDestroy_ );
        m_factoryRenderVertexAttribute = new FactoryPool<OpenGLRenderVertexAttribute, 16>();
        m_factoryRenderFragmentShader = Helper::makeFactoryPoolWithListener<OpenGLRenderFragmentShader, 16>( this, &OpenGLRenderSystem::onRenderFragmentShaderDestroy_ );
        m_factoryRenderVertexShader = Helper::makeFactoryPoolWithListener<OpenGLRenderVertexShader, 16>( this, &OpenGLRenderSystem::onRenderVertexShaderDestroy_ );
        m_factoryRenderProgram = Helper::makeFactoryPoolWithListener<OpenGLRenderProgram, 16>( this, &OpenGLRenderSystem::onRenderProgramDestroy_ );
        m_factoryRenderProgramVariable = Helper::makeFactoryPoolWithListener<OpenGLRenderProgramVariable, 16>( this, &OpenGLRenderSystem::onRenderProgramVariableDestroy_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::_finalizeService()
    {
        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            TextureStage & stage = m_textureStage[i];

            if( stage.texture != nullptr )
            {
                IntrusivePtrBase::intrusive_ptr_release( stage.texture );
                stage.texture = nullptr;
            }
        }

        m_currentProgram = nullptr;
        m_currentProgramVariable = nullptr;

        m_currentIndexBuffer = nullptr;
        m_currentVertexBuffer = nullptr;

        m_deferredCompileVertexShaders.clear();
        m_deferredCompileFragmentShaders.clear();
        m_deferredCompilePrograms.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderIndexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexAttribute );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderFragmentShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgram );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgramVariable );

        m_factoryRenderVertexBuffer = nullptr;
        m_factoryRenderIndexBuffer = nullptr;
        m_factoryRenderImage = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryRenderProgram = nullptr;
        m_factoryRenderProgramVariable = nullptr;

        m_cacheRenderImages.clear();
        m_cacheRenderVertexShaders.clear();
        m_cacheRenderFragmentShaders.clear();
        m_cacheRenderPrograms.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderPlatform OpenGLRenderSystem::getRenderPlatformType() const
    {
#ifndef MENGINE_RENDER_OPENGL_ES
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
    bool OpenGLRenderSystem::createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _depth, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount )
    {
        MENGINE_UNUSED( _bits );
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _depth );
        MENGINE_UNUSED( _waitForVSync );
        MENGINE_UNUSED( _FSAAType );
        MENGINE_UNUSED( _FSAAQuality );
        MENGINE_UNUSED( _MultiSampleCount );

        m_resolution = _resolution;

#ifndef MENGINE_RENDER_OPENGL_ES
        Mengine::initialize_GLEXT();
#endif

        const char * vendorStr = reinterpret_cast<const char *>(glGetString( GL_VENDOR ));
        const char * rendererStr = reinterpret_cast<const char *>(glGetString( GL_RENDERER ));
        const char * versionStr = reinterpret_cast<const char *>(glGetString( GL_VERSION ));
        const char * extensionsStr = reinterpret_cast<const char *>(glGetString( GL_EXTENSIONS ));

        LOGGER_WARNING( "Vendor      : %s", vendorStr );
        LOGGER_WARNING( "Renderer    : %s", rendererStr );
        LOGGER_WARNING( "Version     : %s", versionStr );
        LOGGER_WARNING( "Extensions  : %s", extensionsStr );

        OPENGL_RENDER_CHECK_ERROR();

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
        GLCALL( glDisable, (GL_BLEND) );
        GLCALL( glDisable, (GL_DITHER) );

        GLCALL( glDepthMask, (GL_FALSE) );
        GLCALL( glDepthFunc, (GL_LESS) );

#ifndef MENGINE_RENDER_OPENGL_ES
        GLCALL( glMatrixMode, (GL_MODELVIEW) );
        GLCALL( glLoadIdentity, () );

        GLCALL( glMatrixMode, (GL_PROJECTION) );
        GLCALL( glLoadIdentity, () );

        GLCALL( glMatrixMode, (GL_TEXTURE) );
        GLCALL( glLoadIdentity, () );
#endif

        for( const OpenGLRenderVertexShaderPtr & shader : m_deferredCompileVertexShaders )
        {
            if( shader->compile() == false )
            {
                return false;
            }
        }

        m_deferredCompileVertexShaders.clear();

        for( const OpenGLRenderFragmentShaderPtr & shader : m_deferredCompileFragmentShaders )
        {
            if( shader->compile() == false )
            {
                return false;
            }
        }

        m_deferredCompileFragmentShaders.clear();

        for( const OpenGLRenderProgramPtr & program : m_deferredCompilePrograms )
        {
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
        MENGINE_UNUSED( _image );
        MENGINE_UNUSED( _rect );
        //ToDo!

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setScissor( const Viewport & _viewport )
    {
        mt::mat4f pm;
        mt::mul_m4_m4( pm, m_projectionMatrix, m_viewMatrix );

        mt::vec2f b;
        mt::mul_v2_v2_m4( b, _viewport.begin, pm );

        mt::vec2f e;
        mt::mul_v2_v2_m4( e, _viewport.end, pm );

        mt::vec2f vs = m_viewport.size();

        float bx = (b.x + 1.f) * 0.5f * vs.x;
        float by = (1.f - (b.y + 1.f) * 0.5f) * vs.y;
        float ex = (e.x + 1.f) * 0.5f * vs.x;
        float ey = (1.f - (e.y + 1.f) * 0.5f) * vs.y;

        uint32_t left = (uint32_t)bx;
        uint32_t top = (uint32_t)by;
        uint32_t right = (uint32_t)ex;
        uint32_t bottom = (uint32_t)ey;

        glEnable( GL_SCISSOR_TEST );
        glScissor( left, top, right - left, bottom - top );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::removeScissor()
    {
        glDisable( GL_SCISSOR_TEST );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;

        GLsizei xb = static_cast<GLsizei>(m_viewport.begin.x);
        GLsizei ye = static_cast<GLsizei>(m_viewport.end.y);
        GLsizei w = static_cast<GLsizei>(m_viewport.getWidth());
        GLsizei h = static_cast<GLsizei>(m_viewport.getHeight());

        GLsizei resolution_height = static_cast<GLsizei>(m_resolution.getHeight());

        GLCALL( glViewport, (xb, resolution_height - ye, w, h) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setViewMatrix( const mt::mat4f & _viewMatrix )
    {
        m_viewMatrix = _viewMatrix;

        this->updatePMWMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        m_projectionMatrix = _projectionMatrix;

        this->updatePMWMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture )
    {
        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _texture );
        // To Do
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setWorldMatrix( const mt::mat4f & _worldMatrix )
    {
        m_worldMatrix = _worldMatrix;

        this->updatePMWMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr OpenGLRenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const Char * _doc )
    {
        OpenGLRenderVertexBufferPtr buffer = m_factoryRenderVertexBuffer->createObject( _doc );

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
    RenderIndexBufferInterfacePtr OpenGLRenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const Char * _doc )
    {
        OpenGLRenderIndexBufferPtr buffer = m_factoryRenderIndexBuffer->createObject( _doc );

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
    RenderVertexAttributeInterfacePtr OpenGLRenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const Char * _doc )
    {
        OpenGLRenderVertexAttributePtr vertexAttribute = m_factoryRenderVertexAttribute->createObject( _doc );

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
    RenderFragmentShaderInterfacePtr OpenGLRenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const Char * _doc )
    {
        MENGINE_UNUSED( _compile );

        OpenGLRenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject( _doc );

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
    RenderVertexShaderInterfacePtr OpenGLRenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const Char * _doc )
    {
        MENGINE_UNUSED( _compile );

        OpenGLRenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject( _doc );

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
    RenderProgramInterfacePtr OpenGLRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const Char * _doc )
    {
        OpenGLRenderProgramPtr program = m_factoryRenderProgram->createObject( _doc );

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
        MENGINE_UNUSED( _program );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr OpenGLRenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const Char * _doc )
    {
        OpenGLRenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

        if( variable == nullptr )
        {
            LOGGER_ERROR( "invalid create program variable"
            );

            return nullptr;
        }

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program variable"
            );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::setProgramVariable( const RenderProgramVariableInterfacePtr & _programVariable, const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        m_currentProgramVariable = stdex::intrusive_static_cast<OpenGLRenderProgramVariablePtr>(_programVariable);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::drawIndexedPrimitive( EPrimitiveType _type,
        uint32_t _baseVertexIndex, uint32_t _minIndex,
        uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
    {
        MENGINE_UNUSED( _baseVertexIndex );
        MENGINE_UNUSED( _minIndex );
        MENGINE_UNUSED( _verticesNum );

        if( m_currentIndexBuffer == nullptr || m_currentVertexBuffer == nullptr )
        {
            return;
        }

        if( m_currentProgram->enable() == false )
        {
            return;
        }

        m_currentProgram->bindMatrix( m_worldMatrix, m_viewMatrix, m_projectionMatrix, m_totalWVPMatrix );

        if( m_currentProgramVariable != nullptr )
        {
            if( m_currentProgramVariable->apply( m_currentProgram ) == false )
            {
                return;
            }
        }

        for( uint32_t stageId = 0; stageId != MENGINE_MAX_TEXTURE_STAGES; ++stageId )
        {
            const TextureStage & textureStage = m_textureStage[stageId];

            if( textureStage.texture == nullptr )
            {
#ifdef MENGINE_RENDER_OPENGL_ES
                GLCALL( glActiveTexture, (GL_TEXTURE0 + stageId) );
#else
                GLCALL( glActiveTexture_, (GL_TEXTURE0 + stageId) );
#endif

                GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );

                continue;
            }

            textureStage.texture->bind( stageId );

            if( m_currentProgram->bindTexture( stageId ) == false )
            {
                continue;
            }

            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter) );
        }

        m_currentIndexBuffer->enable();
        m_currentVertexBuffer->enable();

        const RenderVertexAttributeInterfacePtr & vertexAttribute = m_currentProgram->getVertexAttribute();

        vertexAttribute->enable();

        GLenum mode = s_getGLPrimitiveMode( _type );
        const RenderIndex * baseIndex = nullptr;
        const RenderIndex * offsetIndex = baseIndex + _startIndex;

        GLenum indexType = s_getGLIndexType( sizeof( RenderIndex ) );

        GLCALL( glDrawElements, (mode, _indexCount, indexType, reinterpret_cast<const GLvoid *>(offsetIndex)) );

        vertexAttribute->disable();

        m_currentIndexBuffer->disable();
        m_currentVertexBuffer->disable();

        for( uint32_t stageId = 0; stageId != MENGINE_MAX_TEXTURE_STAGES; ++stageId )
        {
            TextureStage & textureStage = m_textureStage[stageId];

            if( textureStage.texture == nullptr )
            {
                break;
            }

#ifdef MENGINE_RENDER_OPENGL_ES
            GLCALL( glActiveTexture, (GL_TEXTURE0 + stageId) );
#else
            GLCALL( glActiveTexture_, (GL_TEXTURE0 + stageId) );
#endif

            GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );
        }

        m_currentProgram->disable();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        TextureStage & tStage = m_textureStage[_stage];

        IntrusivePtrBase::intrusive_ptr_release( tStage.texture );

        if( _texture != nullptr )
        {
            OpenGLRenderImage * texture = stdex::intrusive_get<OpenGLRenderImage *>( _texture );
            IntrusivePtrBase::intrusive_ptr_setup( tStage.texture, texture );
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
        MENGINE_UNUSED( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op )
    {
        const GLenum srcBlendFactor = s_toGLBlendFactor( _src );
        const GLenum dstBlendFactor = s_toGLBlendFactor( _dst );
        const GLenum blendOp = s_toGLBlendFactor( _op );

        GLCALL( glBlendFunc, (srcBlendFactor, dstBlendFactor) );

#ifdef MENGINE_RENDER_OPENGL_ES
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
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderSystem::getTextureMemoryUse() const
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
#ifndef MENGINE_RENDER_OPENGL_ES
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
    void OpenGLRenderSystem::findFormatFromChannels_( EPixelFormat _format, uint32_t _channels, EPixelFormat & _hwFormat, uint32_t & _hwChannels ) const
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
    RenderImageInterfacePtr OpenGLRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc )
    {
        MENGINE_UNUSED( _depth );
        MENGINE_UNUSED( _doc );

        uint32_t hwChannels = 0;
        EPixelFormat hwFormat = PF_UNKNOWN;
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

        OpenGLRenderImagePtr image = m_factoryRenderImage->createObject( _doc );

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
    bool OpenGLRenderSystem::beginScene()
    {
        this->clearFrameBuffer( FBT_COLOR | FBT_DEPTH, 0x00000000, 1.f, 0x00000000 );

        m_currentIndexBuffer = nullptr;
        m_currentVertexBuffer = nullptr;

        m_currentProgram = nullptr;
        m_currentProgramVariable = nullptr;

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
                Detail::get_A_float_from_argb32( _color ))
            );
        }

        if( (_frameBufferTypes & FBT_DEPTH) != 0 )
        {
            frameBufferFlags |= GL_DEPTH_BUFFER_BIT;

            if( m_depthMask == false )
            {
                GLCALL( glDepthMask, (GL_TRUE) );
            }

#ifndef MENGINE_RENDER_OPENGL_ES
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
        MENGINE_UNUSED( _fullscreen );

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
    bool OpenGLRenderSystem::supportTextureFormat( EPixelFormat _format ) const
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
        MENGINE_UNUSED( _fullscreen );

        for( OpenGLRenderImage * image : m_cacheRenderImages )
        {
            image->release();
        }

        for( OpenGLRenderVertexShader * shader : m_cacheRenderVertexShaders )
        {
            shader->release();
        }

        for( OpenGLRenderFragmentShader * shader : m_cacheRenderFragmentShaders )
        {
            shader->release();
        }

        for( OpenGLRenderProgram * program : m_cacheRenderPrograms )
        {
            program->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        for( OpenGLRenderImage * image : m_cacheRenderImages )
        {
            image->reload();
        }

        for( OpenGLRenderVertexShader * shader : m_cacheRenderVertexShaders )
        {
            shader->compile();
        }

        for( OpenGLRenderFragmentShader * shader : m_cacheRenderFragmentShaders )
        {
            shader->compile();
        }

        for( OpenGLRenderProgram * program : m_cacheRenderPrograms )
        {
            program->compile();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setVSync( bool _vSync )
    {
        MENGINE_UNUSED( _vSync );
        //m_windowContext->setVSync( _vSync );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::clear( uint8_t _r, uint8_t _g, uint8_t _b )
    {
        MENGINE_UNUSED( _r );
        MENGINE_UNUSED( _g );
        MENGINE_UNUSED( _b );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc )
    {
        MENGINE_UNUSED( _depth );
        MENGINE_UNUSED( _doc );

        uint32_t hwChannels = 0;
        EPixelFormat hwFormat = PF_UNKNOWN;
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

        OpenGLRenderImagePtr texture = m_factoryRenderImage->createObject( _doc );

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
    RenderTargetInterfacePtr OpenGLRenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const Char * _doc )
    {
        MENGINE_UNUSED( _width );
        MENGINE_UNUSED( _height );
        MENGINE_UNUSED( _channels );
        MENGINE_UNUSED( _format );
        MENGINE_UNUSED( _doc );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr OpenGLRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const Char * _doc )
    {
        MENGINE_UNUSED( _width );
        MENGINE_UNUSED( _height );
        MENGINE_UNUSED( _channels );
        MENGINE_UNUSED( _format );
        MENGINE_UNUSED( _doc );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createRenderTargetImage( const RenderTargetInterfacePtr & _renderTarget, const Char * _doc )
    {
        MENGINE_UNUSED( _renderTarget );
        MENGINE_UNUSED( _doc );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderImageDestroy_( OpenGLRenderImage * _image )
    {
        VectorCacheRenderImages::iterator it_found = std::find( m_cacheRenderImages.begin(), m_cacheRenderImages.end(), _image );

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
        VectorCacheRenderVertexShaders::iterator it_found = std::find( m_cacheRenderVertexShaders.begin(), m_cacheRenderVertexShaders.end(), _vertexShader );

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
        VectorCacheRenderFragmentShaders::iterator it_found = std::find( m_cacheRenderFragmentShaders.begin(), m_cacheRenderFragmentShaders.end(), _fragmentShader );

        if( it_found == m_cacheRenderFragmentShaders.end() )
        {
            return;
        }

        *it_found = m_cacheRenderFragmentShaders.back();
        m_cacheRenderFragmentShaders.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderProgramDestroy_( OpenGLRenderProgram * _program )
    {
        VectorCacheRenderPrograms::iterator it_found = std::find( m_cacheRenderPrograms.begin(), m_cacheRenderPrograms.end(), _program );

        if( it_found == m_cacheRenderPrograms.end() )
        {
            return;
        }

        *it_found = m_cacheRenderPrograms.back();
        m_cacheRenderPrograms.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderProgramVariableDestroy_( OpenGLRenderProgramVariable * _variable )
    {
        VectorCacheRenderProgramVariables::iterator it_found = std::find( m_cacheRenderProgramVariables.begin(), m_cacheRenderProgramVariables.end(), _variable );

        if( it_found == m_cacheRenderProgramVariables.end() )
        {
            return;
        }

        *it_found = m_cacheRenderProgramVariables.back();
        m_cacheRenderProgramVariables.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::updatePMWMatrix_()
    {
        m_totalWVPMatrix = m_worldMatrix * m_viewMatrix * m_projectionMatrix;
    }
}
