#include "MockupRenderSystem.h"

#include "Interface/RenderServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

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

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include "math/uv4.h"

#include <algorithm>

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
        , m_textureMemoryUse( 0U )
        , m_textureCount( 0U )
        , m_vertexBufferEnable( false )
        , m_indexBufferEnable( false )
        , m_waitForVSync( false )
    {
        mt::ident_m4( m_projectionMatrix );
        mt::ident_m4( m_modelViewMatrix );
        mt::ident_m4( m_worldMatrix );
        mt::ident_m4( m_totalWVPInvMatrix );
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

        LOGGER_INFO( "render", "Initializing MockupRenderSystem..." );

        m_dxMaxCombinedTextureImageUnits = MENGINE_MAX_TEXTURE_STAGES;

        m_renderSystemName = CONFIG_VALUE( "Engine", "MockupRenderSystem", STRINGIZE_STRING_LOCAL( "DX9" ) );

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
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::createRenderWindow( const Resolution & _resolution
        , uint32_t _bits
        , bool _fullscreen
        , bool _depth
        , bool _waitForVSync
        , int32_t _FSAAType
        , int32_t _FSAAQuality
        , uint32_t _MultiSampleCount )
    {
        MENGINE_UNUSED( _bits );
        MENGINE_UNUSED( _FSAAType );
        MENGINE_UNUSED( _FSAAQuality );
        MENGINE_UNUSED( _MultiSampleCount );

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_fullscreen = _fullscreen;
        m_depth = _depth;
        m_waitForVSync = _waitForVSync;

        LOGGER_MESSAGE( "render initalized successfully!" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        float Engine_DX9PerfectPixelOffsetX = CONFIG_VALUE( "Engine", "DX9PerfectPixelOffsetX", -0.5f );
        float Engine_DX9PerfectPixelOffsetY = CONFIG_VALUE( "Engine", "DX9PerfectPixelOffsetY", -0.5f );

        float perfect_x = Engine_DX9PerfectPixelOffsetX / (m_windowViewport.end.x - m_windowViewport.begin.x);
        float perfect_y = Engine_DX9PerfectPixelOffsetY / (m_windowViewport.end.y - m_windowViewport.begin.y);

        mt::mat4f vmperfect;
        mt::make_translation_m4( vmperfect, perfect_x, perfect_y, 0.f );

        mt::mul_m4_m4( m_projectionMatrix, _projectionMatrix, vmperfect );

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
    RenderImageInterfacePtr MockupRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "render", "texture normal created %dx%d %d:%d depth %d"
            , _width
            , _height
            , _format
            , _channels
            , _depth
        );

        MockupRenderImagePtr dxTexture = this->createRenderImage_( _mipmaps, _width, _height, _channels, _depth, _format, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dxTexture, "invalid create render texture" );

        return dxTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MockupRenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MockupRenderTargetTexturePtr target = m_factoryRenderTargetTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( target );

        if( target->initialize( _width, _height, _channels, _format ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target %dx%d format %d"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "offscreen target created %dx%d %d"
            , _width
            , _height
            , _format
        );

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MockupRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MockupRenderTargetOffscreenPtr target = m_factoryRenderTargetOffscreen->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( target );

        if( target->initialize( _width, _height, _channels, _format ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target %dx%d format %d"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "offscreen target created %dx%d %d"
            , _width
            , _height
            , _format
        );

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MockupRenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentPtr & _doc )
    {
        MockupRenderTargetTexturePtr targetTexture = stdex::intrusive_static_cast<MockupRenderTargetTexturePtr>(_renderTarget);

        MockupRenderImageTargetPtr imageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageTarget );

        imageTarget->initialize( targetTexture );

        return imageTarget;
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
    void MockupRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, float _depth, uint32_t _stencil )
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
            LOGGER_ERROR( "Graphics change mode failed"
            );
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
        return m_textureMemoryUse;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderSystem::getTextureCount() const
    {
        return m_textureCount;
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer MockupRenderSystem::getRenderSystemExtention()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::supportTextureFormat( EPixelFormat _format ) const
    {
        MENGINE_UNUSED( _format );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderSystem::supportTextureNonPow2() const
    {
        return true;
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
            LOGGER_ERROR( "no support stage %d (max %d)"
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
            LOGGER_ERROR( "release resources"
            );

            return false;
        }

        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        RENDER_SERVICE()
            ->onDeviceLostRestore();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::release_()
    {
        if( this->releaseResources_() == false )
        {
            LOGGER_ERROR( "invalid release resource"
            );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr MockupRenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentPtr & _doc )
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

        _vertexBuffer->enable();

        m_vertexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr MockupRenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentPtr & _doc )
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

        _indexBuffer->enable();

        m_indexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::drawIndexedPrimitive( EPrimitiveType _type, uint32_t _vertexBase
        , uint32_t _minIndex, uint32_t _vertexCount, uint32_t _indexStart, uint32_t _indexCount )
    {
        MENGINE_UNUSED( _type );
        MENGINE_UNUSED( _vertexBase );
        MENGINE_UNUSED( _minIndex );
        MENGINE_UNUSED( _vertexCount );
        MENGINE_UNUSED( _indexStart );
        MENGINE_UNUSED( _indexCount );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        MENGINE_UNUSED( _program );

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "no support stage %d (max %d)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        if( _texture != nullptr )
        {
            _texture->bind( _stage );

            m_textureEnable[_stage] = true;
        }
        else
        {
            m_textureEnable[_stage] = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op )
    {
        MENGINE_UNUSED( _src );
        MENGINE_UNUSED( _dst );
        MENGINE_UNUSED( _op );

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
            LOGGER_ERROR( "no support stage %d (max %d)"
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
            LOGGER_ERROR( "no support stage %d (max %d)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr MockupRenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentPtr & _doc )
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
    RenderFragmentShaderInterfacePtr MockupRenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc )
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
    RenderVertexShaderInterfacePtr MockupRenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc )
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
    RenderProgramInterfacePtr MockupRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc )
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
    RenderProgramVariableInterfacePtr MockupRenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentPtr & _doc )
    {
        MockupRenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( variable, "invalid create program variable"
        );

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program variable"
            );

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
            LOGGER_ERROR( "Graphics change mode failed"
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImagePtr MockupRenderSystem::createRenderImage_( uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, uint32_t _hwDepth, EPixelFormat _hwPixelFormat, const DocumentPtr & _doc )
    {
        MockupRenderImagePtr dx9RenderImage = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dx9RenderImage );

        dx9RenderImage->initialize( _mipmaps, _hwWidth, _hwHeight, _hwChannels, _hwDepth, _hwPixelFormat );

#ifdef MENGINE_DEBUG
        bool logcreateimage = HAS_OPTION( "logcreateimage" );

        if( logcreateimage == true )
        {
            LOGGER_STATISTIC( "create texture size %u:%u channels %u format %u (doc %s)"
                , _hwWidth
                , _hwHeight
                , _hwChannels
                , _hwPixelFormat
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }
#endif

#ifdef MENGINE_DEBUG
        ++m_textureCount;

        uint32_t memoryUse = Helper::getTextureMemorySize( _hwWidth, _hwHeight, _hwChannels, _hwDepth, _hwPixelFormat );

        m_textureMemoryUse += memoryUse;
#endif

        return dx9RenderImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderSystem::onDestroyRenderImage_( MockupRenderImage * _image )
    {
        MENGINE_UNUSED( _image );

        _image->finalize();

#ifdef MENGINE_DEBUG
        m_textureCount--;

        uint32_t hwWidth = _image->getHWWidth();
        uint32_t hwHeight = _image->getHWHeight();
        uint32_t hwChannels = _image->getHWChannels();
        EPixelFormat hwPixelFormat = _image->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, 1, hwPixelFormat );

        m_textureMemoryUse -= memoryUse;
#endif
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

        mt::transpose_m4_m4( m_totalWVPInvMatrix, totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
}

