#include "MockupRenderSystem.h"

#include "Interface/RenderServiceInterface.h"

#include "MockupRenderImage.h"
#include "MockupRenderImageTarget.h"
#include "MockupRenderTargetTexture.h"
#include "MockupRenderTargetOffscreen.h"

#include "MockupRenderVertexShader.h"
#include "MockupRenderFragmentShader.h"
#include "MockupRenderProgram.h"
#include "MockupRenderVertexAttribute.h"
#include "MockupRenderVertexBuffer.h"
#include "MockupRenderIndexBuffer.h"
#include "MockupRenderProgramVariable.h"

#include "MockupRenderImageLockedFactoryStorage.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/Algorithm.h"

#include "math/uv4.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MockupRenderSystem, Mengine::MockupRenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderSystem::MockupRenderSystem()
        : m_fullscreen( true )
        , m_depth( false )
        , m_frames( 0 )
        , m_dxMaxCombinedTextureImageUnits( 0 )
        , m_vertexBufferEnable( false )
        , m_indexBufferEnable( false )
        , m_waitForVSync( false )
    {
        mt::ident_m4( &m_projectionMatrix );
        mt::ident_m4( &m_modelViewMatrix );
        mt::ident_m4( &m_worldMatrix );
        mt::ident_m4( &m_totalWVPInvMatrix );

        Algorithm::fill_n( m_textureEnable, MENGINE_MAX_TEXTURE_STAGES, false );
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderSystem::~MockupRenderSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderPlatform MockupRenderSystem::getRenderPlatformType() const
    {
        return RP_DX9;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MockupRenderSystem::getRenderPlatformName() const
    {
        return m_renderSystemName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::_initializeService()
    {
        m_frames = 0;

        m_dxMaxCombinedTextureImageUnits = MENGINE_MAX_TEXTURE_STAGES;

        m_renderSystemName = CONFIG_VALUE( "Engine", "MockupRenderSystem", STRINGIZE_STRING_LOCAL( "Mockup" ) );

        m_factoryRenderVertexAttribute = Helper::makeFactoryPool<MockupRenderVertexAttribute, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderVertexShader = Helper::makeFactoryPool<MockupRenderVertexShader, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderFragmentShader = Helper::makeFactoryPool<MockupRenderFragmentShader, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgram = Helper::makeFactoryPoolWithListener<MockupRenderProgram, 64>( this, &MockupRenderSystem::onDestroyRenderProgram_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgramVariable = Helper::makeFactoryPool<MockupRenderProgramVariable, 64>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryVertexBuffer = Helper::makeFactoryPool<MockupRenderVertexBuffer, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryIndexBuffer = Helper::makeFactoryPool<MockupRenderIndexBuffer, 8>( MENGINE_DOCUMENT_FACTORABLE );

        m_factoryRenderImage = Helper::makeFactoryPoolWithListener<MockupRenderImage, 128>( this, &MockupRenderSystem::onDestroyRenderImage_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderImageTarget = Helper::makeFactoryPoolWithListener<MockupRenderImageTarget, 16>( this, &MockupRenderSystem::onDestroyRenderImageTarget_, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryRenderTargetTexture = Helper::makeFactoryPoolWithListener<MockupRenderTargetTexture, 16>( this, &MockupRenderSystem::onDestroyRenderTargetTexture_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderTargetOffscreen = Helper::makeFactoryPoolWithListener<MockupRenderTargetOffscreen, 16>( this, &MockupRenderSystem::onDestroyRenderTargetOffscreen_, MENGINE_DOCUMENT_FACTORABLE );

        MockupRenderImageLockedFactoryStorage::initialize( Helper::makeFactoryPool<MockupRenderImageLocked, 8>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::_finalizeService()
    {
        this->release_();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexAttribute );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderFragmentShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgram );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgramVariable );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryVertexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryIndexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImageTarget );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderTargetTexture );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderTargetOffscreen );

        m_factoryRenderVertexAttribute = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderProgram = nullptr;
        m_factoryRenderProgramVariable = nullptr;
        m_factoryVertexBuffer = nullptr;
        m_factoryIndexBuffer = nullptr;
        m_factoryRenderImage = nullptr;
        m_factoryRenderImageTarget = nullptr;
        m_factoryRenderTargetTexture = nullptr;
        m_factoryRenderTargetOffscreen = nullptr;

        MockupRenderImageLockedFactoryStorage::finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::createRenderWindow( const RenderWindowDesc * _windowDesc )
    {
        m_windowResolution = _windowDesc->resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_fullscreen = _windowDesc->fullscreen;
        m_depth = _windowDesc->depth;
        m_waitForVSync = _windowDesc->waitForVSync;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        m_projectionMatrix = _projectionMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setViewMatrix( const mt::mat4f & _modelViewMatrix )
    {
        m_modelViewMatrix = _modelViewMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setWorldMatrix( const mt::mat4f & _worldMatrix )
    {
        m_worldMatrix = _worldMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MockupRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "render", "texture normal created %ux%u format %x"
            , _width
            , _height
            , _format
        );

        MockupRenderImagePtr dxTexture = this->createRenderImage_( _mipmaps, _width, _height, _format, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dxTexture, "invalid create render texture" );

        return dxTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MockupRenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MockupRenderTargetTexturePtr target = m_factoryRenderTargetTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( target );

        if( target->initialize( _width, _height, _format ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target [%ux%u] format [%u]"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "offscreen target created [%ux%u] format [%u]"
            , _width
            , _height
            , _format
        );

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MockupRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MockupRenderTargetOffscreenPtr target = m_factoryRenderTargetOffscreen->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( target );

        if( target->initialize( _width, _height, _format ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target [%ux%u] format [%u]"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "offscreen target created [%ux%u] format [%u]"
            , _width
            , _height
            , _format
        );

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MockupRenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentInterfacePtr & _doc )
    {
        MockupRenderTargetTexturePtr targetTexture = MockupRenderTargetTexturePtr::from( _renderTarget );

        MockupRenderImageTargetPtr imageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageTarget );

        imageTarget->initialize( targetTexture );

        return imageTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialStageCacheInterfacePtr MockupRenderSystem::createRenderMaterialStageCache( const RenderMaterialStage * _stage, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _doc );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::beginScene()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::endScene()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::swapBuffers()
    {
        ++m_frames;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil )
    {
        MENGINE_UNUSED( _frameBufferTypes );
        MENGINE_UNUSED( _color );
        MENGINE_UNUSED( _depth );
        MENGINE_UNUSED( _stencil );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setScissor( const Viewport & _viewport )
    {
        MENGINE_UNUSED( _viewport );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::removeScissor()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_windowResolution == _resolution && m_fullscreen == _fullscreen )
        {
            return;
        }

        m_windowResolution = _resolution;
        m_fullscreen = _fullscreen;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        if( this->restore_() == false )
        {
            LOGGER_ASSERTION( "graphics change mode failed" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderSystem::getAvailableTextureMemory() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderSystem::getTextureMemoryUse() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderSystem::getTextureCount() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderSystem::getMaxCombinedTextureImageUnits() const
    {
        return m_dxMaxCombinedTextureImageUnits;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onWindowMovedOrResized()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onWindowClose()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _matrix )
    {
        MENGINE_UNUSED( _matrix );

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage [%u] (max %u)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::releaseResources_()
    {
        m_vertexBufferEnable = false;
        m_indexBufferEnable = false;

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            m_textureEnable[i] = false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::restore_()
    {
        if( this->releaseResources_() == false )
        {
            LOGGER_ASSERTION( "release resources" );

            return false;
        }

        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        if( RENDER_SERVICE()
            ->onDeviceLostRestore() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::release_()
    {
        if( this->releaseResources_() == false )
        {
            LOGGER_ASSERTION( "invalid release resource" );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr MockupRenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        MockupRenderVertexBufferPtr buffer = m_factoryVertexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer );

        if( buffer->initialize( _vertexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        if( _vertexBuffer == nullptr )
        {
            if( m_vertexBufferEnable == false )
            {
                return true;
            }

            m_vertexBufferEnable = false;

            return true;
        }

        m_vertexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr MockupRenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        MockupRenderIndexBufferPtr buffer = m_factoryIndexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer );

        if( buffer->initialize( _indexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        if( _indexBuffer == nullptr )
        {
            if( m_indexBufferEnable == false )
            {
                return true;
            }

            m_indexBufferEnable = false;

            return true;
        }

        m_indexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc )
    {
        MENGINE_UNUSED( _stageCache );
        MENGINE_UNUSED( _desc );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        MENGINE_UNUSED( _program );

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage [%u] (max %u)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        if( _texture != nullptr )
        {
            m_textureEnable[_stage] = true;
        }
        else
        {
            m_textureEnable[_stage] = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate )
    {
        MENGINE_UNUSED( _src );
        MENGINE_UNUSED( _dst );
        MENGINE_UNUSED( _op );
        MENGINE_UNUSED( _separateSrc );
        MENGINE_UNUSED( _separateDst );
        MENGINE_UNUSED( _separateOp );
        MENGINE_UNUSED( _separate );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        MENGINE_UNUSED( _modeU );
        MENGINE_UNUSED( _modeV );
        MENGINE_UNUSED( _border );

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage [%u] (max %u)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setTextureFactor( uint32_t _color )
    {
        MENGINE_UNUSED( _color );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setCullMode( ECullMode _mode )
    {
        MENGINE_UNUSED( _mode );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        MENGINE_UNUSED( _depthTest );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        MENGINE_UNUSED( _depthWrite );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        MENGINE_UNUSED( _depthFunction );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setFillMode( EFillMode _mode )
    {
        MENGINE_UNUSED( _mode );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        MENGINE_UNUSED( _r );
        MENGINE_UNUSED( _g );
        MENGINE_UNUSED( _b );
        MENGINE_UNUSED( _a );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        MENGINE_UNUSED( _alphaBlend );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        MENGINE_UNUSED( _minification );
        MENGINE_UNUSED( _mipmap );
        MENGINE_UNUSED( _magnification );

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage [%u] (max %u)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr MockupRenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentInterfacePtr & _doc )
    {
        MockupRenderVertexAttributePtr attribute = m_factoryRenderVertexAttribute->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( attribute, "invalid create attribute '%s'"
            , _name.c_str()
        );

        if( attribute->initialize( _name, _elementSize ) == false )
        {
            LOGGER_ERROR( "invalid initialize attribute '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return attribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr MockupRenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        MockupRenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create shader '%s'"
            , _name.c_str()
        );

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr MockupRenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        MockupRenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create shader '%s'"
            , _name.c_str()
        );

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr MockupRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _samplerCount );

        MockupRenderProgramPtr program = m_factoryRenderProgram->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( program, "invalid create program '%s'"
            , _name.c_str()
        );

        if( program->initialize( _name, _vertex, _fragment, _vertexAttribute ) == false )
        {
            LOGGER_ERROR( "invalid initialize program '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr MockupRenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc )
    {
        MockupRenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( variable, "invalid create program variable"
        );

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program variable" );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable )
    {
        MENGINE_UNUSED( _program );
        MENGINE_UNUSED( _variable );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setVSync( bool _vSync )
    {
        if( m_waitForVSync == _vSync )
        {
            return;
        }

        m_waitForVSync = _vSync;

        if( this->restore_() == false )
        {
            LOGGER_ASSERTION( "graphics change mode failed" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImagePtr MockupRenderSystem::createRenderImage_( uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, EPixelFormat _hwPixelFormat, const DocumentInterfacePtr & _doc )
    {
        MockupRenderImagePtr dx9RenderImage = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dx9RenderImage );

        dx9RenderImage->initialize( _mipmaps, _hwWidth, _hwHeight, _hwPixelFormat );

#if defined(MENGINE_DEBUG)
        bool OPTION_logcreateimage = HAS_OPTION( "logcreateimage" );

        if( OPTION_logcreateimage == true )
        {
            LOGGER_STATISTIC( "create texture size %u:%u format %x (doc %s)"
                , _hwWidth
                , _hwHeight
                , _hwPixelFormat
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }
#endif

        return dx9RenderImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onDestroyRenderImage_( MockupRenderImage * _image )
    {
        MENGINE_UNUSED( _image );

        _image->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onDestroyRenderProgram_( MockupRenderProgram * _program )
    {
        _program->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onDestroyRenderImageTarget_( MockupRenderImageTarget * _imageTarget )
    {
        _imageTarget->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onDestroyRenderTargetTexture_( MockupRenderTargetTexture * _targetTexture )
    {
        _targetTexture->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onDestroyRenderTargetOffscreen_( MockupRenderTargetOffscreen * _targetOffscreen )
    {
        _targetOffscreen->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::updateWVPInvMatrix_()
    {
        mt::mat4f totalWVPMatrix = m_worldMatrix * m_modelViewMatrix * m_projectionMatrix;

        mt::transpose_m4_m4( &m_totalWVPInvMatrix, totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
}

