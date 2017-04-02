#   include "OpenGLRenderSystem.h"
#   include "OpenGLRenderEnum.h"
#   include "OpenGLRenderError.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/PlatformInterface.h"

#   include "Factory/FactoryDefault.h"
#   include "Factory/FactoryPool.h"

#   include "Logger/Logger.h"

#   include "OpenGL.h"

#   include <cmath>

//////////////////////////////////////////////////////////////////////////
#   define GET_A_FLOAT_FROM_ARGB32( argb ) ( ((float)(argb >> 24)) / 255.0f )
#   define GET_R_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 16) & 0xFF)) / 255.0f )
#   define GET_G_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 8) & 0xFF)) / 255.0f )
#   define GET_B_FLOAT_FROM_ARGB32( argb ) ( (float)(argb & 0xFF) / 255.0f )
//////////////////////////////////////////////////////////////////////////
#   define GLUNUSED(x) ((void)x)
//////////////////////////////////////////////////////////////////////////

SERVICE_FACTORY( RenderSystem, Menge::OpenGLRenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderSystem::OpenGLRenderSystem()
        : m_window( nullptr )
        , m_activeTextureStage( 0 )
        , m_activeTexture( 0 )
        , m_depthMask( false )
        , m_glMaxClipPlanes( 0 )
        , m_glMaxCombinedTextureImageUnits( 0 )
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
        LOGGER_WARNING( m_serviceProvider )("Initializing OpenGL RenderSystem...");

        m_renderPlatform = STRINGIZE_STRING_LOCAL( m_serviceProvider, "OpenGL" );

        m_factoryVertexBuffer = new FactoryDefault<OpenGLRenderVertexBuffer>( m_serviceProvider );
        m_factoryIndexBuffer = new FactoryDefault<OpenGLRenderIndexBuffer>( m_serviceProvider );
        m_factoryRenderImage = new FactoryPool<OpenGLRenderImage, 128>( m_serviceProvider );
        m_factoryRenderFragmentShader = new FactoryPool<OpenGLRenderFragmentShader, 16>( m_serviceProvider );
        m_factoryRenderVertexShader = new FactoryPool<OpenGLRenderVertexShader, 16>( m_serviceProvider );
        m_factoryProgram = new FactoryPool<OpenGLRenderProgram, 16>( m_serviceProvider );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::_finalize()
    {
        m_currentProgram = nullptr;

        m_currentIndexBuffer = nullptr;
        m_currentVertexBuffer = nullptr;

        m_factoryVertexBuffer = nullptr;
        m_factoryIndexBuffer = nullptr;
        m_factoryRenderImage = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryProgram = nullptr;    

        m_fragmentShaders.clear();
        m_vertexShaders.clear();
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

        Menge::initialize_GLEXT();

        GLint ver_major = 0, ver_minor = 0;
        glGetIntegerv( GL_MAJOR_VERSION, &ver_major );
        glGetIntegerv( GL_MINOR_VERSION, &ver_minor );

        LOGGER_WARNING( m_serviceProvider )("OpenGL Version: %d.%d", ver_major, ver_minor);

        OPENGL_RENDER_CHECK_ERROR( m_serviceProvider );

        LOGGER_WARNING( m_serviceProvider )("Vendor      : %s", reinterpret_cast<const char*>(glGetString( GL_VENDOR )));
        LOGGER_WARNING( m_serviceProvider )("Renderer    : %s", reinterpret_cast<const char*>(glGetString( GL_RENDERER )));
        LOGGER_WARNING( m_serviceProvider )("Version     : %s", reinterpret_cast<const char*>(glGetString( GL_VERSION )));
        LOGGER_WARNING( m_serviceProvider )("Extensions  : %s", reinterpret_cast<const char*>(glGetString( GL_EXTENSIONS )));

        OPENGL_RENDER_CHECK_ERROR( m_serviceProvider );

        m_window = PLATFORM_SERVICE( m_serviceProvider )
            ->getWindowHandle();

        GLint maxClipPlanes;
        glGetIntegerv( GL_MAX_CLIP_PLANES, &maxClipPlanes );

        m_glMaxClipPlanes = maxClipPlanes;

        GLint maxCombinedTextureImageUnits;
        glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits );

        m_glMaxCombinedTextureImageUnits = maxCombinedTextureImageUnits;

        for( uint32_t i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
        {
            GLCALL( m_serviceProvider, glActiveTexture_, (GL_TEXTURE0 + i) );
            GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE) );
            GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
            GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR) );
            GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
            GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA) );
            GLCALL( m_serviceProvider, glDisable, (GL_TEXTURE_2D) );

            m_textureStage[i] = TextureStage();
        }

        GLCALL( m_serviceProvider, glFrontFace, (GL_CW) );
        GLCALL( m_serviceProvider, glDisable, (GL_DEPTH_TEST) );
        GLCALL( m_serviceProvider, glDisable, (GL_STENCIL_TEST) );
        GLCALL( m_serviceProvider, glDisable, (GL_CULL_FACE) );
        GLCALL( m_serviceProvider, glDisable, (GL_LIGHTING) );
        GLCALL( m_serviceProvider, glDisable, (GL_BLEND) );
        GLCALL( m_serviceProvider, glDisable, (GL_ALPHA_TEST) );
        GLCALL( m_serviceProvider, glDisable, (GL_DITHER) );

        GLCALL( m_serviceProvider, glDepthMask, (GL_FALSE) );

        GLCALL( m_serviceProvider, glMatrixMode, (GL_MODELVIEW) );
        GLCALL( m_serviceProvider, glLoadIdentity, () );

        GLCALL( m_serviceProvider, glMatrixMode, (GL_PROJECTION) );
        GLCALL( m_serviceProvider, glLoadIdentity, () );

        GLCALL( m_serviceProvider, glMatrixMode, (GL_TEXTURE) );
        GLCALL( m_serviceProvider, glLoadIdentity, () );

        GLCALL( m_serviceProvider, glDisableClientState, (GL_VERTEX_ARRAY) );
        GLCALL( m_serviceProvider, glDisableClientState, (GL_NORMAL_ARRAY) );
        GLCALL( m_serviceProvider, glDisableClientState, (GL_COLOR_ARRAY) );
        GLCALL( m_serviceProvider, glDisableClientState, (GL_TEXTURE_COORD_ARRAY) );

        m_resolution = _resolution;

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

        GLCALL( m_serviceProvider, glViewport, (xb, height - yb, w, h) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
    {
        m_projectionMatrix = _projection;
        glMatrixMode( GL_PROJECTION );
        glLoadMatrixf( m_projectionMatrix.buff() );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setModelViewMatrix( const mt::mat4f & _view )
    {
        m_viewMatrix = _view;
        glMatrixMode( GL_MODELVIEW );
        glLoadMatrixf( m_viewMatrix.buff() );
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
    RenderVertexBufferInterfacePtr OpenGLRenderSystem::createVertexBuffer( uint32_t _verticesNum, bool _dynamic )
    {
        MarmaladeRenderVertexBufferPtr buffer = m_factoryVertexBuffer->createObject();

        buffer->setServiceProvider( m_serviceProvider );

        if( buffer->initialize( _verticesNum, _dynamic ) == false )
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
    RenderIndexBufferInterfacePtr OpenGLRenderSystem::createIndexBuffer( uint32_t _indiciesNum, bool _dynamic )
    {
        OpenGLRenderIndexBufferPtr buffer = m_factoryIndexBuffer->createObject();

        buffer->setServiceProvider( m_serviceProvider );

        if( buffer->initialize( _indiciesNum, _dynamic ) == false )
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
    RenderFragmentShaderInterfacePtr OpenGLRenderSystem::createFragmentShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
    {
        OpenGLRenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject();

        shader->setServiceProvider( m_serviceProvider );

        if( shader->initialize( _name, _buffer, _size, _isCompile ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createFragmentShader invalid initialize shader %s"
                , _name.c_str()
                );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr OpenGLRenderSystem::createVertexShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
    {
        OpenGLRenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject();

        shader->setServiceProvider( m_serviceProvider );

        if( shader->initialize( _name, _buffer, _size, _isCompile ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createVertexShader invalid initialize shader %s"
                , _name.c_str()
                );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr OpenGLRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, uint32_t _samplerCount )
    {
        OpenGLProgramPtr program = m_factoryProgram->createObject();

        program->setServiceProvider( m_serviceProvider );

        if( program->initialize( _name, _vertex, _fragment, _samplerCount ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createProgram invalid initialize program %s"
                , _name.c_str()
                );

            return nullptr;
        }

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        m_currentProgram = stdex::intrusive_static_cast<OpenGLProgramPtr>(_program);
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        GLUNUSED( _program );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setupCombiners_( const TextureStage & _textureStage )
    {
        GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_COMBINE_RGB, _textureStage.colorOp) );
        GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SOURCE0_RGB, _textureStage.colorArg1) );
        GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SOURCE1_RGB, _textureStage.colorArg2) );

        GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, _textureStage.alphaOp) );
        GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, _textureStage.alphaArg1) );
        GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, _textureStage.alphaArg2) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::drawIndexedPrimitive( EPrimitiveType _type,
        uint32_t _baseVertexIndex, uint32_t _minIndex,
        uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
    {
        GLUNUSED( _verticesNum );
        GLUNUSED( _minIndex );
        GLUNUSED( _baseVertexIndex );

        if( m_currentIndexBuffer == nullptr || m_currentVertexBuffer == nullptr )
        {
            return;
        }

        if( m_currentProgram != nullptr )
        {
            m_currentProgram->enable();

            m_currentProgram->bindMatrix( m_worldMatrix, m_viewMatrix, m_projectionMatrix );
        }

        for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
        {
            const TextureStage & textureStage = m_textureStage[i];

            GLCALL( m_serviceProvider, glActiveTexture_, (GL_TEXTURE0 + i) );

            if( textureStage.texture == nullptr )
            {
                GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, 0) );
                GLCALL( m_serviceProvider, glDisable, (GL_TEXTURE_2D) );

                continue;
            }

            GLCALL( m_serviceProvider, glEnable, (GL_TEXTURE_2D) );

            GLuint texture_uid = textureStage.texture->getUId();

            GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, texture_uid) );

            if( m_currentProgram != nullptr )
            {
                m_currentProgram->bindTexture( i );
            }

            GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS) );
            GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT) );
            GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter) );
            GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter) );

            this->setupCombiners_( textureStage );
        }

        OpenGLRenderIndexBuffer * ib = stdex::intrusive_get<OpenGLRenderIndexBuffer *>( m_currentIndexBuffer );
        ib->enable();

        OpenGLRenderVertexBuffer * vb = stdex::intrusive_get<OpenGLRenderVertexBuffer *>( m_currentVertexBuffer );
        vb->enable();

        GLCALL( m_serviceProvider, glEnableClientState, (GL_VERTEX_ARRAY) );
        GLCALL( m_serviceProvider, glEnableClientState, (GL_COLOR_ARRAY) );

        GLCALL( m_serviceProvider, glVertexPointer, (3, GL_FLOAT, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, position ))) );
        GLCALL( m_serviceProvider, glColorPointer, (4, GL_UNSIGNED_BYTE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, color ))) );

        for( uint32_t i = 0; i != MENGINE_RENDER_VERTEX_UV_COUNT; ++i )
        {
            const TextureStage & textureStage = m_textureStage[i];
            
            if( textureStage.texture == nullptr )
            {
                continue;
            }
            
            GLCALL( m_serviceProvider, glClientActiveTexture_, (GL_TEXTURE0 + i) );
            GLCALL( m_serviceProvider, glEnableClientState, (GL_TEXTURE_COORD_ARRAY) );

            const ptrdiff_t uv_offset = offsetof( RenderVertex2D, uv ) + sizeof( mt::vec2f ) * i;
            GLCALL( m_serviceProvider, glTexCoordPointer, (2, GL_FLOAT, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(uv_offset)) );
        }

        GLenum mode = s_getGLPrimitiveMode( _type );
        const RenderIndices * baseIndex = nullptr;
        const RenderIndices * offsetIndex = baseIndex + _startIndex;

        GLenum indexType = s_getGLIndexType();

        GLCALL( m_serviceProvider, glDrawElements, (mode, _indexCount, indexType, reinterpret_cast<const GLvoid *>(offsetIndex)) );

        GLCALL( m_serviceProvider, glDisableClientState, (GL_VERTEX_ARRAY) );
        GLCALL( m_serviceProvider, glDisableClientState, (GL_COLOR_ARRAY) );

        for( uint32_t i = 0; i != MENGINE_RENDER_VERTEX_UV_COUNT; ++i )
        {
            GLCALL( m_serviceProvider, glClientActiveTexture_, (GL_TEXTURE0 + i) );
            GLCALL( m_serviceProvider, glDisableClientState, (GL_TEXTURE_COORD_ARRAY) );
        }

        for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
        {
            TextureStage & textureStage = m_textureStage[i];

            if( textureStage.texture == nullptr )
            {
                break;
            }

            GLCALL( m_serviceProvider, glActiveTexture_, (GL_TEXTURE0 + i) );

            GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, 0) );
        }

        if( m_currentProgram != nullptr )
        {
            m_currentProgram->disable();
            m_currentProgram = nullptr;
        }

        GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, 0) );
        GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        TextureStage & tStage = m_textureStage[_stage];

        if( _texture != nullptr )
        {
            OpenGLRenderImage * texture = stdex::intrusive_get<OpenGLRenderImage *>( _texture );
            stdex::intrusive_ptr_setup( tStage.texture, texture );
        }
        else
        {
            stdex::intrusive_ptr_release( tStage.texture );
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
    //////////////////////////////////////////////////////////////////////////
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

        GLCALL( m_serviceProvider, glBlendFunc, (srcBlendFactor, dstBlendFactor) );
        GLCALL( m_serviceProvider, glBlendEquation_, (blendOp) );
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

        GLCALL( m_serviceProvider, glCullFace, (face) );

        if( enable == true )
        {
            GLCALL( m_serviceProvider, glEnable, (GL_CULL_FACE) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, (GL_CULL_FACE) );
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
            GLCALL( m_serviceProvider, glEnable, ( GL_DEPTH_TEST ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_DEPTH_TEST ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        m_depthMask = _depthWrite;

        GLCALL( m_serviceProvider, glDepthMask, ( m_depthMask ? GL_TRUE : GL_FALSE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        const GLenum cmpFunc = s_toGLCmpFunc( _depthFunction );
        GLCALL( m_serviceProvider, glDepthFunc, ( cmpFunc ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setFillMode( EFillMode _mode )
    {
        const GLenum mode = s_getGLFillMode(_mode);
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        GLCALL( m_serviceProvider, glColorMask, ( _r ? GL_TRUE : GL_FALSE, _g ? GL_TRUE : GL_FALSE, _b ? GL_TRUE : GL_FALSE, _a ? GL_TRUE : GL_FALSE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setShadeType( EShadeType _sType )
    {
        const GLenum model = s_toGLShadeMode( _sType );
        GLCALL( m_serviceProvider, glShadeModel, ( model ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        if( _alphaBlend == true )
        {
            GLCALL( m_serviceProvider, glEnable, ( GL_BLEND ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_BLEND ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setLightingEnable( bool _light )
    {
        if( _light == true )
        {
            GLCALL( m_serviceProvider, glEnable, ( GL_LIGHTING ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_LIGHTING ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
    {
        if( _textrueOp == Menge::TOP_SELECTARG2 )
        {
            _arg1 = _arg2;
            _textrueOp = Menge::TOP_SELECTARG1;
        }

        m_textureStage[_stage].colorOp = s_getGLTextureOp( _textrueOp );
        m_textureStage[_stage].colorArg1 = s_getGLTextureArg( _arg1 );
        m_textureStage[_stage].colorArg2 = s_getGLTextureArg( _arg2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
    {
        if( _textrueOp == Menge::TOP_SELECTARG2 )
        {
            _arg1 = _arg2;
            _textrueOp = Menge::TOP_SELECTARG1;
        }

        m_textureStage[_stage].alphaOp = s_getGLTextureOp( _textrueOp );
        m_textureStage[_stage].alphaArg1 = s_getGLTextureArg( _arg1 );
        m_textureStage[_stage].alphaArg2 = s_getGLTextureArg( _arg2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index )
    {
        GLUNUSED(_stage);
        GLUNUSED(_index);
        //m_textureStage[_stage].texCoordIndex = _index;
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
                    _hwFormat = PF_X8R8G8B8;
                    _hwChannels = 4;
                }
                else if( _channels == 4 )
                {
                    _hwFormat = PF_A8R8G8B8;
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
        GLUNUSED(_depth);

        uint32_t hwChannels = 0;
        PixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

        GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

        if( textureInternalFormat == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystem::createImage invalid get GL Texture Internal format for PF %d"
                , hwFormat
                );

            return nullptr;
        }

        GLint textureColorFormat = s_toGLColorFormat( hwFormat );

        if( textureColorFormat == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystem::createImage invalid get GL Texture Color format for PF %d"
                , hwFormat
                );

            return nullptr;
        }

        GLint textureColorDataType = s_getGLColorDataType( hwFormat );

        if( textureColorDataType == 0 )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createImage invalid get GL Color Data Type for PF %d"
                , hwFormat
                );

            return nullptr;
        }

        OpenGLRenderImagePtr texture = m_factoryRenderImage->createObject();

        if( texture == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createImage invalid create"
                );

            return nullptr;
        }

        texture->setServiceProvider( m_serviceProvider );

        if( texture->initialize( ERIM_NORMAL
            , _mipmaps
            , _width
            , _height
            , hwChannels
            , hwFormat
            , textureInternalFormat
            , textureColorFormat
            , textureColorDataType ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createImage invalid initialize"
                );

            return nullptr;
        }

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::lockRenderTarget( const RenderImageInterfacePtr & _renderTarget )
    {
        GLUNUSED(_renderTarget);
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
        this->clearFrameBuffer( FBT_COLOR, 0, 1.f, 0 );

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
        GLCALL( m_serviceProvider, SDL_GL_SwapWindow, (m_window) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
    {
        GLbitfield frameBufferFlags = 0;
        
        if( ( _frameBufferTypes & FBT_COLOR ) != 0 )
        {
            frameBufferFlags |= GL_COLOR_BUFFER_BIT;

            GLCALL( m_serviceProvider, glClearColor, ( 
                GET_R_FLOAT_FROM_ARGB32( _color ), 
                GET_G_FLOAT_FROM_ARGB32( _color ), 
                GET_B_FLOAT_FROM_ARGB32( _color ), 
                GET_A_FLOAT_FROM_ARGB32( _color ) )
                );
        }

        if( ( _frameBufferTypes & FBT_DEPTH ) != 0 )
        {
            frameBufferFlags |= GL_DEPTH_BUFFER_BIT;

            if( m_depthMask == false )
            {
                GLCALL( m_serviceProvider, glDepthMask, ( GL_TRUE ) );
            }

            GLCALL( m_serviceProvider, glClearDepth, (static_cast<GLclampd>(_depth)) );
        }

        if( (_frameBufferTypes & FBT_STENCIL) != 0 )
        {
            frameBufferFlags |= GL_STENCIL_BUFFER_BIT;

            GLCALL( m_serviceProvider, glClearStencil, ( _stencil ) );
        }

        GLCALL( m_serviceProvider, glClear, ( frameBufferFlags ) );

        if( m_depthMask == false )
        {
            GLCALL( m_serviceProvider, glDepthMask, ( GL_FALSE ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        GLUNUSED(_fullscreen);

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
    void OpenGLRenderSystem::setVSync( bool _vSync )
    {
        GLUNUSED(_vSync);
        //m_windowContext->setVSync( _vSync );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::clear( uint32_t _color, bool _force )
    {
        GLUNUSED(_color);
        GLUNUSED(_force);
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
    {
        GLUNUSED(_depth);

        uint32_t hwChannels = 0;
        PixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

        GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

        if( textureInternalFormat == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystem::createDynamicImage invalid get GL Texture Internal format for PF %d"
                , hwFormat
                );

            return nullptr;
        }

        GLint textureColorFormat = s_toGLColorFormat( hwFormat );

        if( textureColorFormat == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystem::createDynamicImage invalid get GL Texture Color format for PF %d"
                , hwFormat
                );

            return nullptr;
        }

        GLint textureColorDataType = s_getGLColorDataType( hwFormat );

        if( textureColorDataType == 0 )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createDynamicImage invalid get GL Color Data Type for PF %d"
                , hwFormat
                );

            return nullptr;
        }

        OpenGLRenderImagePtr texture = m_factoryRenderImage->createObject();

        texture->setServiceProvider( m_serviceProvider );

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
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystem::createDynamicImage invalid initialize"
                );

            return nullptr;
        }

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterface * OpenGLRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format )
    {
        GLUNUSED(_width);
        GLUNUSED(_height);
        GLUNUSED(_format);

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
        mt::make_projection_ortho_lh_m4(ortho, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

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
        mt::make_projection_frustum_m4(frustum, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

        mt::mul_m4_m4( _projectionMatrix, transform, frustum );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport )
    {
        GLUNUSED(_viewport);

        mt::mat4f wm;
        mt::ident_m4( wm );

        mt::inv_m4_m4( _viewMatrix, wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
    {
        mt::make_lookat_m4( _viewMatrix, _eye, _dir, _up, _sign );
    }
}   // namespace Menge
