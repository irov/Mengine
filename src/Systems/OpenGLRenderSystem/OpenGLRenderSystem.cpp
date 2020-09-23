#include "OpenGLRenderSystem.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderError.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/ConstStringHelper.h"

#include "Kernel/Logger.h"

#include <cmath>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::OpenGLRenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderSystem::OpenGLRenderSystem()
        : m_glMaxCombinedTextureImageUnits( 0 )
        , m_renderWindowCreate( false )
        , m_depthMask( false )
#ifdef MENGINE_RENDER_OPENGL
        , m_vertexArrayId( 0 )
#endif
        , m_counterTexture( 0 )
        , m_counterFramebuffer( 0 )
        , m_counterBuffer( 0 )
    {
        mt::ident_m4( m_worldMatrix );
        mt::ident_m4( m_viewMatrix );
        mt::ident_m4( m_projectionMatrix );
        mt::ident_m4( m_totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderSystem::~OpenGLRenderSystem()
    {
#ifdef MENGINE_RENDER_OPENGL
        MENGINE_ASSERTION_FATAL( m_vertexArrayId == 0 );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::_initializeService()
    {
        LOGGER_MESSAGE( "Initializing OpenGL RenderSystem..." );

#ifndef MENGINE_RENDER_OPENGL_ES
        m_renderPlatform = STRINGIZE_STRING_LOCAL( "OpenGL" );
#else
        m_renderPlatform = STRINGIZE_STRING_LOCAL( "OpenGLES" );
#endif

        m_factoryRenderVertexBuffer = Helper::makeFactoryPoolWithListener<OpenGLRenderVertexBuffer, 8>( this, &OpenGLRenderSystem::onRenderVertexBufferDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderIndexBuffer = Helper::makeFactoryPoolWithListener<OpenGLRenderIndexBuffer, 8>( this, &OpenGLRenderSystem::onRenderIndexBufferDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderImage = Helper::makeFactoryPoolWithListener<OpenGLRenderImage, 128>( this, &OpenGLRenderSystem::onRenderImageDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderImageTarget = Helper::makeFactoryPoolWithListener<OpenGLRenderImageTarget, 128>( this, &OpenGLRenderSystem::onRenderImageTargetDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderTargetTexture = Helper::makeFactoryPoolWithListener<OpenGLRenderTargetTexture, 128>( this, &OpenGLRenderSystem::onRenderTargetTextureDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderVertexAttribute = Helper::makeFactoryPool<OpenGLRenderVertexAttribute, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderFragmentShader = Helper::makeFactoryPoolWithListener<OpenGLRenderFragmentShader, 16>( this, &OpenGLRenderSystem::onRenderFragmentShaderDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderVertexShader = Helper::makeFactoryPoolWithListener<OpenGLRenderVertexShader, 16>( this, &OpenGLRenderSystem::onRenderVertexShaderDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgram = Helper::makeFactoryPoolWithListener<OpenGLRenderProgram, 16>( this, &OpenGLRenderSystem::onRenderProgramDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgramVariable = Helper::makeFactoryPool<OpenGLRenderProgramVariable, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::_finalizeService()
    {
#ifdef MENGINE_RENDER_OPENGL
        if( m_vertexArrayId != 0 )
        {
            GLCALL( glDeleteVertexArrays, (1, &m_vertexArrayId) );
            m_vertexArrayId = 0;
        }
#endif

        m_currentProgram = nullptr;
        m_currentProgramVariable = nullptr;
        m_currentVertexAttribute = nullptr;
        m_currentIndexBuffer = nullptr;
        m_currentVertexBuffer = nullptr;

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            TextureStage & stage = m_textureStage[i];

            if( stage.texture != nullptr )
            {
                IntrusivePtrBase::intrusive_ptr_release( stage.texture );
                stage.texture = nullptr;
            }
        }

        m_deferredCompilePrograms.clear();

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_cacheRenderIndexBuffers );
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_cacheRenderVertexBuffers );
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_cacheRenderImages );
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_cacheRenderImageTargets );
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_cacheRenderTargetTextures );
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_cacheRenderPrograms );

        m_cacheRenderIndexBuffers.clear();
        m_cacheRenderVertexBuffers.clear();
        m_cacheRenderImages.clear();
        m_cacheRenderImageTargets.clear();
        m_cacheRenderTargetTextures.clear();
        m_cacheRenderPrograms.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderIndexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImageTarget );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderTargetTexture );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexAttribute );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderFragmentShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgram );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgramVariable );

        m_factoryRenderVertexBuffer = nullptr;
        m_factoryRenderIndexBuffer = nullptr;
        m_factoryRenderImage = nullptr;
        m_factoryRenderImageTarget = nullptr;
        m_factoryRenderTargetTexture = nullptr;
        m_factoryRenderVertexAttribute = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryRenderProgram = nullptr;
        m_factoryRenderProgramVariable = nullptr;

        MENGINE_ASSERTION_FATAL( m_counterTexture == 0 );
        MENGINE_ASSERTION_FATAL( m_counterFramebuffer == 0 );
        MENGINE_ASSERTION_FATAL( m_counterBuffer == 0 );
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

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

#ifndef MENGINE_RENDER_OPENGL_ES
        Mengine::initialize_GLEXT();
#endif

        LOGGER_MESSAGE_RELEASE( "OpenGL driver properties" );

        const Char * vendorStr = reinterpret_cast<const Char *>(glGetString( GL_VENDOR ));
        LOGGER_MESSAGE_RELEASE( "Vendor      : %s", vendorStr );
        OPENGL_RENDER_CHECK_ERROR();

        const Char * rendererStr = reinterpret_cast<const Char *>(glGetString( GL_RENDERER ));
        LOGGER_MESSAGE_RELEASE( "Renderer    : %s", rendererStr );
        OPENGL_RENDER_CHECK_ERROR();

        const Char * versionStr = reinterpret_cast<const Char *>(glGetString( GL_VERSION ));
        LOGGER_MESSAGE_RELEASE( "Version     : %s", versionStr );
        OPENGL_RENDER_CHECK_ERROR();

#ifdef MENGINE_RENDER_OPENGL_ES
        const Char * extensionsStr = reinterpret_cast<const Char *>(glGetString( GL_EXTENSIONS ));
        LOGGER_MESSAGE_RELEASE( "Extensions  : %s", extensionsStr );
        OPENGL_RENDER_CHECK_ERROR();
#endif

        const Char * shadingLanguageVersion = reinterpret_cast<const Char *>(glGetString( GL_SHADING_LANGUAGE_VERSION ));
        LOGGER_MESSAGE_RELEASE( "Shading Language Version     : %s", shadingLanguageVersion );
        OPENGL_RENDER_CHECK_ERROR();

        GLint maxCombinedTextureImageUnits;
        glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits );

        m_glMaxCombinedTextureImageUnits = maxCombinedTextureImageUnits;

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
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

#ifdef MENGINE_RENDER_OPENGL
        GLuint vertexArrayId;
        GLCALL( glGenVertexArrays, (1, &vertexArrayId) );

        if( vertexArrayId == 0 )
        {
            return false;
        }

        m_vertexArrayId = vertexArrayId;
#endif

        for( const OpenGLRenderProgramPtr & program : m_deferredCompilePrograms )
        {
            if( program->compile() == false )
            {
                return false;
            }

            OpenGLRenderProgram * program_ptr = program.get();
            m_cacheRenderPrograms.push_back( program_ptr );
        }

        m_deferredCompilePrograms.clear();

        m_renderWindowCreate = true;

        return true;
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
        float by = (1.f - (1.f - (b.y + 1.f) * 0.5f)) * vs.y;
        float ex = (e.x + 1.f) * 0.5f * vs.x;
        float ey = (1.f - (1.f - (e.y + 1.f) * 0.5f)) * vs.y;

        uint32_t left = (uint32_t)bx;
        uint32_t top = (uint32_t)by;
        uint32_t right = (uint32_t)ex;
        uint32_t bottom = (uint32_t)ey;

        uint32_t width = right - left;
        uint32_t height = top - bottom;

        glEnable( GL_SCISSOR_TEST );
        glScissor( left, bottom, width, height );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::removeScissor()
    {
        glDisable( GL_SCISSOR_TEST );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setViewport( const Viewport & _viewport )
    {
        if( m_viewport.equalViewport( _viewport ) == true )
        {
            return;
        }

        m_viewport = _viewport;

        GLsizei x = static_cast<GLsizei>(m_viewport.begin.x + 0.5f);
        GLsizei y = static_cast<GLsizei>(m_viewport.getHeight() - m_viewport.end.y + 0.5f);
        GLsizei w = static_cast<GLsizei>(m_viewport.getWidth() + 0.5f);
        GLsizei h = static_cast<GLsizei>(m_viewport.getHeight() + 0.5f);

        GLCALL( glViewport, (x, y, w, h) );
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
        float OpenGL_PerfectPixelOffsetX = CONFIG_VALUE( "OpenGL", "PerfectPixelOffsetX", 0.f );
        float OpenGL_PerfectPixelOffsetY = CONFIG_VALUE( "OpenGL", "PerfectPixelOffsetY", 0.f );

        if( OpenGL_PerfectPixelOffsetX != 0.f || OpenGL_PerfectPixelOffsetY != 0.f )
        {
            float perfect_x = OpenGL_PerfectPixelOffsetX / (m_windowViewport.end.x - m_windowViewport.begin.x);
            float perfect_y = OpenGL_PerfectPixelOffsetY / (m_windowViewport.end.y - m_windowViewport.begin.y);

            mt::mat4f vmperfect;
            mt::make_translation_m4( vmperfect, perfect_x, perfect_y, 0.f );

            mt::mul_m4_m4( m_projectionMatrix, _projectionMatrix, vmperfect );
        }
        else
        {
            m_projectionMatrix = _projectionMatrix;
        }

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
    RenderVertexBufferInterfacePtr OpenGLRenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentPtr & _doc )
    {
        OpenGLRenderVertexBufferPtr buffer = m_factoryRenderVertexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create render vertex buffer (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( buffer->initialize( _vertexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        OpenGLRenderVertexBuffer * buffer_ptr = buffer.get();
        m_cacheRenderVertexBuffers.push_back( buffer_ptr );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        m_currentVertexBuffer = _vertexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr OpenGLRenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentPtr & _doc )
    {
        OpenGLRenderIndexBufferPtr buffer = m_factoryRenderIndexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create render index buffer (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( buffer->initialize( _indexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        OpenGLRenderIndexBuffer * buffer_ptr = buffer.get();
        m_cacheRenderIndexBuffers.push_back( buffer_ptr );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        m_currentIndexBuffer = _indexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr OpenGLRenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentPtr & _doc )
    {
        OpenGLRenderVertexAttributePtr vertexAttribute = m_factoryRenderVertexAttribute->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute, "invalid create vertex attribute '%s'"
            , _name.c_str()
        );

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
    RenderFragmentShaderInterfacePtr OpenGLRenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _compile );

        OpenGLRenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create shader '%s'"
            , _name.c_str()
        );

        if( shader->initialize( _name, _memory ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr OpenGLRenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _compile );

        OpenGLRenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create shader '%s'"
            , _name.c_str()
        );

        if( shader->initialize( _name, _memory ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr OpenGLRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc )
    {
        OpenGLRenderProgramPtr program = m_factoryRenderProgram->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( program, "invalid create program '%s'"
            , _name.c_str()
        );

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

            OpenGLRenderProgram * program_ptr = program.get();
            m_cacheRenderPrograms.push_back( program_ptr );
        }
        else
        {
            m_deferredCompilePrograms.push_back( program );
        }

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        m_currentProgram = _program;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        //OpenGLRenderProgramPtr ogl_program = stdex::intrusive_static_cast<OpenGLRenderProgramPtr>(_program);

        //ogl_program->bindMatrix( m_worldMatrix, m_viewMatrix, m_projectionMatrix, m_totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr OpenGLRenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentPtr & _doc )
    {
        OpenGLRenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( variable, "invalid create program variable" );

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program variable" );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _programVariable )
    {
        MENGINE_UNUSED( _program );

        m_currentProgramVariable = _programVariable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::drawIndexedPrimitive( EPrimitiveType _type
        , uint32_t _baseVertexIndex
        , uint32_t _minIndex
        , uint32_t _verticesNum
        , uint32_t _startIndex
        , uint32_t _indexCount )
    {
        MENGINE_UNUSED( _baseVertexIndex );
        MENGINE_UNUSED( _minIndex );
        MENGINE_UNUSED( _verticesNum );

        MENGINE_ASSERTION_FATAL( m_currentIndexBuffer != nullptr );
        MENGINE_ASSERTION_FATAL( m_currentVertexBuffer != nullptr );
        MENGINE_ASSERTION_FATAL( m_currentProgram != nullptr );

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

            RenderImageInterface * texture = textureStage.texture;

            if( texture == nullptr )
            {
#ifdef MENGINE_RENDER_OPENGL_ES
                GLCALL( glActiveTexture, (GL_TEXTURE0 + stageId) );
#else
                GLCALL( glActiveTexture_, (GL_TEXTURE0 + stageId) );
#endif

                GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );

                continue;
            }

            texture->bind( stageId );

            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter) );
            GLCALL( glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter) );

            if( m_currentProgram->bindTexture( stageId ) == false )
            {
                continue;
            }
        }

#ifdef MENGINE_RENDER_OPENGL
        GLCALL( glBindVertexArray, (m_vertexArrayId) );
#endif

        m_currentIndexBuffer->enable();
        m_currentVertexBuffer->enable();

        const RenderVertexAttributeInterfacePtr & vertexAttribute = m_currentProgram->getVertexAttribute();

        vertexAttribute->enable();

        GLenum mode = Helper::toGLPrimitiveMode( _type );
        GLenum indexType = Helper::toGLIndexType( sizeof( RenderIndex ) );
        const GLvoid * indices = reinterpret_cast<const GLvoid *>(_startIndex * sizeof( RenderIndex ));

        GLCALL( glDrawElements, (mode, _indexCount, indexType, indices) );

        vertexAttribute->disable();

        m_currentIndexBuffer->disable();
        m_currentVertexBuffer->disable();

        for( uint32_t stageId = 0; stageId != MENGINE_MAX_TEXTURE_STAGES; ++stageId )
        {
            TextureStage & textureStage = m_textureStage[stageId];

            RenderImageInterface * texture = textureStage.texture;

            if( texture == nullptr )
            {
                break;
            }

            textureStage.texture->unbind( stageId );
        }

        m_currentProgram->disable();

#ifdef MENGINE_RENDER_OPENGL
        GLCALL( glBindVertexArray, (0) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stageId, const RenderImageInterfacePtr & _texture )
    {
        MENGINE_UNUSED( _program );

        TextureStage & tStage = m_textureStage[_stageId];

        IntrusivePtrBase::intrusive_ptr_release( tStage.texture );

        if( _texture != nullptr )
        {
            IntrusivePtrBase::intrusive_ptr_setup( tStage.texture, _texture.get() );
        }
        else
        {
            tStage.texture = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        GLenum modeUGL = Helper::toGLAddressMode( _modeU );
        GLenum modeVGL = Helper::toGLAddressMode( _modeV );

        m_textureStage[_stage].wrapS = modeUGL;
        m_textureStage[_stage].wrapT = modeVGL;
        m_textureStage[_stage].border = _border;
    }
    //////////////////////////////////////////L////////////////////////////////
    void OpenGLRenderSystem::setTextureFactor( uint32_t _color )
    {
        MENGINE_UNUSED( _color );

        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op )
    {
        const GLenum srcBlendFactor = Helper::toGLBlendFactor( _src );
        const GLenum dstBlendFactor = Helper::toGLBlendFactor( _dst );
        const GLenum blendOp = Helper::toGLBlendOp( _op );

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
    UnknownPointer OpenGLRenderSystem::getRenderSystemExtention()
    {
        return this;
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
        GLenum cmpFunc = Helper::toGLCmpFunc( _depthFunction );

        GLCALL( glDepthFunc, (cmpFunc) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setFillMode( EFillMode _mode )
    {
        MENGINE_UNUSED( _mode );

#ifndef MENGINE_RENDER_OPENGL_ES
        GLenum mode = Helper::toGLFillMode( _mode );

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

        tStage.minFilter = Helper::toGLMinFilter( _minification, _mipmap );
        tStage.magFilter = Helper::toMagFilter( _magnification );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::findFormatFromChannels_( EPixelFormat _format, uint32_t _channels, EPixelFormat * const _hwFormat, uint32_t * const _hwChannels ) const
    {
        switch( _format )
        {
        case PF_UNKNOWN:
            {
                if( _channels == 1 )
                {
                    *_hwFormat = PF_A8;
                    *_hwChannels = 1;
                }
                else if( _channels == 3 )
                {
                    *_hwFormat = PF_X8R8G8B8;    //original
                    *_hwChannels = 4;            // original
                }
                else if( _channels == 4 )
                {
                    *_hwFormat = PF_A8R8G8B8; //original
                    *_hwChannels = 4;
                }
            }break;
        default:
            {
                *_hwFormat = _format;
                *_hwChannels = _channels;
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _depth );

        uint32_t hwChannels = 0;
        EPixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, _channels, &hwFormat, &hwChannels );

        GLint textureInternalFormat = Helper::toGLInternalFormat( hwFormat );

        MENGINE_ASSERTION_FATAL( textureInternalFormat != 0, "invalid get GL Texture Internal format for PF %d"
            , hwFormat
        );

        GLint textureColorFormat = Helper::toGLColorFormat( hwFormat );

        MENGINE_ASSERTION_FATAL( textureColorFormat != 0, "invalid get GL Texture Color format for PF %d"
            , hwFormat
        );

        GLint textureColorDataType = Helper::toGLColorDataType( hwFormat );

        MENGINE_ASSERTION_FATAL( textureColorDataType != 0, "invalid get GL Color Data Type for PF %d"
            , hwFormat
        );

        OpenGLRenderImagePtr image = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( image, "invalid create" );

        if( image->initialize( _mipmaps
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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::endScene()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::swapBuffers()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, float _depth, uint32_t _stencil )
    {
        MENGINE_UNUSED( _depth );

        GLbitfield frameBufferFlags = 0;

        if( (_frameBufferTypes & FBT_COLOR) != 0 )
        {
            frameBufferFlags |= GL_COLOR_BUFFER_BIT;

            if( m_clearColor != _color )
            {
                m_clearColor = _color;

                GLclampf r = m_clearColor.getR();
                GLclampf g = m_clearColor.getG();
                GLclampf b = m_clearColor.getB();
                GLclampf a = m_clearColor.getA();

                GLCALL( glClearColor, (r, g, b, a) );
            }
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

        if( (_frameBufferTypes & FBT_DEPTH) != 0 )
        {
            if( m_depthMask == false )
            {
                GLCALL( glDepthMask, (GL_FALSE) );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_viewport = Viewport( 0.f, 0.f, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::supportTextureFormat( EPixelFormat _format ) const
    {
        return Helper::toGLInternalFormat( _format ) != 0;
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
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onWindowClose()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

#ifdef MENGINE_RENDER_OPENGL
        if( m_vertexArrayId != 0 )
        {
            GLCALL( glDeleteVertexArrays, (1, &m_vertexArrayId) );
            m_vertexArrayId = 0;
        }
#endif

        for( OpenGLRenderIndexBuffer * buffer : m_cacheRenderIndexBuffers )
        {
            buffer->release();
        }

        for( OpenGLRenderVertexBuffer * buffer : m_cacheRenderVertexBuffers )
        {
            buffer->release();
        }

        for( OpenGLRenderImage * image : m_cacheRenderImages )
        {
            image->release();
        }

        for( OpenGLRenderTargetTexture * target : m_cacheRenderTargetTextures )
        {
            target->release();
        }

        for( OpenGLRenderProgram * program : m_cacheRenderPrograms )
        {
            program->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

#ifdef MENGINE_RENDER_OPENGL
        GLuint vertexArrayId;
        GLCALL( glGenVertexArrays, (1, &vertexArrayId) );

        if( vertexArrayId == 0 )
        {
            return false;
        }

        m_vertexArrayId = vertexArrayId;
#endif

        for( OpenGLRenderIndexBuffer * buffer : m_cacheRenderIndexBuffers )
        {
            if( buffer->reload() == false )
            {
                return false;
            }
        }

        for( OpenGLRenderVertexBuffer * buffer : m_cacheRenderVertexBuffers )
        {
            if( buffer->reload() == false )
            {
                return false;
            }
        }

        for( OpenGLRenderImage * image : m_cacheRenderImages )
        {
            if( image->reload() == false )
            {
                return false;
            }
        }

        for( OpenGLRenderTargetTexture * target : m_cacheRenderTargetTextures )
        {
            if( target->reload() == false )
            {
                return false;
            }
        }

        for( OpenGLRenderProgram * program : m_cacheRenderPrograms )
        {
            if( program->compile() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::setVSync( bool _vSync )
    {
        MENGINE_UNUSED( _vSync );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr OpenGLRenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _width );
        MENGINE_UNUSED( _height );
        MENGINE_UNUSED( _channels );
        MENGINE_UNUSED( _format );
        MENGINE_UNUSED( _doc );

        uint32_t hwChannels = 0;
        EPixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, _channels, &hwFormat, &hwChannels );

        GLint textureInternalFormat = Helper::toGLInternalFormat( hwFormat );

        MENGINE_ASSERTION_FATAL( textureInternalFormat != 0, "invalid get GL Texture Internal format for PF %d"
            , hwFormat
        );

        GLint textureColorFormat = Helper::toGLColorFormat( hwFormat );

        MENGINE_ASSERTION_FATAL( textureColorFormat != 0, "invalid get GL Texture Color format for PF %d"
            , hwFormat
        );

        GLint textureColorDataType = Helper::toGLColorDataType( hwFormat );

        MENGINE_ASSERTION_FATAL( textureColorDataType != 0, "invalid get GL Color Data Type for PF %d"
            , hwFormat
        );

        OpenGLRenderTargetTexturePtr renderTarget = m_factoryRenderTargetTexture->createObject( _doc );

        if( renderTarget->initialize( _width, _height, hwChannels, hwFormat, textureInternalFormat, textureColorFormat, textureColorDataType ) == false )
        {
            LOGGER_ERROR( "invalid initialize"
            );

            return nullptr;
        }

        OpenGLRenderTargetTexture * renderTarget_ptr = renderTarget.get();
        m_cacheRenderTargetTextures.push_back( renderTarget_ptr );

        return renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr OpenGLRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _width );
        MENGINE_UNUSED( _height );
        MENGINE_UNUSED( _channels );
        MENGINE_UNUSED( _format );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr OpenGLRenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentPtr & _doc )
    {
        OpenGLRenderTargetTexturePtr targetTexture = stdex::intrusive_static_cast<OpenGLRenderTargetTexturePtr>(_renderTarget);

        OpenGLRenderImageTargetPtr imageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageTarget );

        imageTarget->initialize( targetTexture );

        return imageTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderVertexBufferDestroy_( OpenGLRenderVertexBuffer * _buffer )
    {
        _buffer->finalize();

        VectorCacheRenderVertexBuffers::iterator it_found = std::find( m_cacheRenderVertexBuffers.begin(), m_cacheRenderVertexBuffers.end(), _buffer );

        if( it_found == m_cacheRenderVertexBuffers.end() )
        {
            return;
        }

        *it_found = m_cacheRenderVertexBuffers.back();
        m_cacheRenderVertexBuffers.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderIndexBufferDestroy_( OpenGLRenderIndexBuffer * _buffer )
    {
        _buffer->finalize();

        VectorCacheRenderIndexBuffers::iterator it_found = std::find( m_cacheRenderIndexBuffers.begin(), m_cacheRenderIndexBuffers.end(), _buffer );

        if( it_found == m_cacheRenderIndexBuffers.end() )
        {
            return;
        }

        *it_found = m_cacheRenderIndexBuffers.back();
        m_cacheRenderIndexBuffers.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderImageDestroy_( OpenGLRenderImage * _image )
    {
        _image->finalize();

        VectorCacheRenderImages::iterator it_found = std::find( m_cacheRenderImages.begin(), m_cacheRenderImages.end(), _image );

        if( it_found == m_cacheRenderImages.end() )
        {
            return;
        }

        *it_found = m_cacheRenderImages.back();
        m_cacheRenderImages.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderImageTargetDestroy_( OpenGLRenderImageTarget * _image )
    {
        _image->finalize();

        VectorCacheRenderImageTargets::iterator it_found = std::find( m_cacheRenderImageTargets.begin(), m_cacheRenderImageTargets.end(), _image );

        if( it_found == m_cacheRenderImageTargets.end() )
        {
            return;
        }

        *it_found = m_cacheRenderImageTargets.back();
        m_cacheRenderImageTargets.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderTargetTextureDestroy_( OpenGLRenderTargetTexture * _renderTarget )
    {
        _renderTarget->finalize();

        VectorCacheRenderTargetTextures::iterator it_found = std::find( m_cacheRenderTargetTextures.begin(), m_cacheRenderTargetTextures.end(), _renderTarget );

        if( it_found == m_cacheRenderTargetTextures.end() )
        {
            return;
        }

        *it_found = m_cacheRenderTargetTextures.back();
        m_cacheRenderTargetTextures.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderVertexShaderDestroy_( OpenGLRenderVertexShader * _vertexShader )
    {
        MENGINE_ASSERTION_FATAL( _vertexShader->isCompile() == false );

        _vertexShader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderFragmentShaderDestroy_( OpenGLRenderFragmentShader * _fragmentShader )
    {
        MENGINE_ASSERTION_FATAL( _fragmentShader->isCompile() == false );

        _fragmentShader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::onRenderProgramDestroy_( OpenGLRenderProgram * _program )
    {
        VectorCacheRenderPrograms::iterator it_found = std::find( m_cacheRenderPrograms.begin(), m_cacheRenderPrograms.end(), _program );

        if( it_found == m_cacheRenderPrograms.end() )
        {
            _program->finalize();

            return;
        }

        _program->release();

        MENGINE_ASSERTION_FATAL( _program->isCompile() == false );

        _program->finalize();

        *it_found = m_cacheRenderPrograms.back();
        m_cacheRenderPrograms.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::updatePMWMatrix_()
    {
        m_totalWVPMatrix = m_worldMatrix * m_viewMatrix * m_projectionMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderSystem::genTexture()
    {
        GLuint id = 0;
        GLCALL( glGenTextures, (1, &id) );

        ++m_counterTexture;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::deleteTexture( GLuint _id )
    {
        GLCALL( glDeleteTextures, (1, &_id) );

        --m_counterTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderSystem::genFramebuffer()
    {
        GLuint id = 0;
        GLCALL( glGenFramebuffers, (1, &id) );

        ++m_counterFramebuffer;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::deleteFramebuffer( GLuint _id )
    {
        GLCALL( glDeleteFramebuffers, (1, &_id) );

        --m_counterFramebuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderSystem::genBuffer()
    {
        GLuint id = 0;
        GLCALL( glGenBuffers, (1, &id) );

        ++m_counterBuffer;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystem::deleteBuffer( GLuint _id )
    {
        GLCALL( glDeleteBuffers, (1, &_id) );

        --m_counterBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
