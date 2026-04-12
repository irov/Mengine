#include "MetalRenderSystem.h"

#include "MetalRenderImageLockedFactoryStorage.h"

#include "MetalRenderEnum.h"

#include "MetalRenderVertexShader.h"
#include "MetalRenderFragmentShader.h"

#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

#include "Interface/PlatformServiceInterface.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionStatistic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/TextureHelper.h"

#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"

#import <Metal/Metal.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::MetalRenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderSystem::MetalRenderSystem()
        : m_maxCombinedTextureImageUnits( 0 )
        , m_maxTextureSize( 0 )
        , m_renderWindowCreate( false )
        , m_renderDeviceLost( false )
        , m_depthMask( false )
        , m_depthTest( false )
        , m_alphaBlend( false )
        , m_clearDepth( 1.0 )
        , m_clearStencil( 0 )
        , m_cullMode( CM_CULL_NONE )
        , m_fillMode( FM_SOLID )
        , m_depthFunction( CMPF_LESS )
        , m_blendSrc( BF_ONE )
        , m_blendDst( BF_ZERO )
        , m_blendOp( BOP_ADD )
        , m_blendSeparateSrc( BF_ONE )
        , m_blendSeparateDst( BF_ZERO )
        , m_blendSeparateOp( BOP_ADD )
        , m_blendSeparate( false )
        , m_colorMaskR( true )
        , m_colorMaskG( true )
        , m_colorMaskB( true )
        , m_colorMaskA( true )
        , m_device( nil )
        , m_commandQueue( nil )
        , m_commandBuffer( nil )
        , m_renderEncoder( nil )
        , m_depthStencilState( nil )
        , m_depthStencilTexture( nil )
        , m_depthStencilWidth( 0 )
        , m_depthStencilHeight( 0 )
        , m_currentDrawable( nil )
        , m_currentDrawableTexture( nil )
    {
        mt::ident_m4( &m_worldMatrix );
        mt::ident_m4( &m_viewMatrix );
        mt::ident_m4( &m_projectionMatrix );
        mt::ident_m4( &m_totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderSystem::~MetalRenderSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::_initializeService()
    {
        m_renderPlatform = STRINGIZE_STRING_LOCAL( "Metal" );

        m_factoryRenderVertexBuffer = Helper::makeFactoryPoolWithListener<MetalRenderVertexBuffer, 8>( this, &MetalRenderSystem::onRenderVertexBufferDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderIndexBuffer = Helper::makeFactoryPoolWithListener<MetalRenderIndexBuffer, 8>( this, &MetalRenderSystem::onRenderIndexBufferDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderImage = Helper::makeFactoryPoolWithListener<MetalRenderImage, 128>( this, &MetalRenderSystem::onRenderImageDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderImageTarget = Helper::makeFactoryPoolWithListener<MetalRenderImageTarget, 128>( this, &MetalRenderSystem::onRenderImageTargetDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderTargetTexture = Helper::makeFactoryPoolWithListener<MetalRenderTargetTexture, 128>( this, &MetalRenderSystem::onRenderTargetTextureDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderVertexAttribute = Helper::makeFactoryPool<MetalRenderVertexAttribute, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderFragmentShader = Helper::makeFactoryPoolWithListener<MetalRenderFragmentShader, 16>( this, &MetalRenderSystem::onRenderFragmentShaderDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderVertexShader = Helper::makeFactoryPoolWithListener<MetalRenderVertexShader, 16>( this, &MetalRenderSystem::onRenderVertexShaderDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgram = Helper::makeFactoryPoolWithListener<MetalRenderProgram, 16>( this, &MetalRenderSystem::onRenderProgramDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgramVariable = Helper::makeFactoryPool<MetalRenderProgramVariable, 16>( MENGINE_DOCUMENT_FACTORABLE );

        MetalRenderImageLockedFactoryStorage::initialize( Helper::makeFactoryPool<MetalRenderImageLocked, 16>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::_finalizeService()
    {
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

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_renderResourceHandlers );

        m_renderResourceHandlers.clear();

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

        m_renderEncoder = nil;
        m_commandBuffer = nil;
        m_commandQueue = nil;
        m_depthStencilState = nil;
        m_depthStencilTexture = nil;
        m_currentDrawable = nil;
        m_currentDrawableTexture = nil;
        m_device = nil;

        MetalRenderImageLockedFactoryStorage::finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderPlatform MetalRenderSystem::getRenderPlatformType() const
    {
        return RP_METAL;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderSystem::getRenderPlatformName() const
    {
        return m_renderPlatform;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::createRenderWindow( const RenderWindowDesc * _windowDesc )
    {
        m_windowResolution = _windowDesc->resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        iOSPlatformServiceExtensionInterface * iOSPlatformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        m_device = iOSPlatformExtension->getMetalDevice();

        if( m_device == nil )
        {
            LOGGER_ERROR( "failed to get Metal device from platform" );

            return false;
        }

        LOGGER_INFO( "metal", "Metal device name: %s"
            , [[m_device name] UTF8String]
        );

        m_commandQueue = [m_device newCommandQueue];

        if( m_commandQueue == nil )
        {
            LOGGER_ERROR( "failed to create Metal command queue" );

            return false;
        }

        m_maxCombinedTextureImageUnits = MENGINE_MAX_TEXTURE_STAGES;

#if TARGET_OS_OSX
        if( [m_device supportsFamily:MTLGPUFamilyApple1] == YES )
        {
            m_maxTextureSize = 16384;
        }
        else
        {
            m_maxTextureSize = 8192;
        }
#else
        if( [m_device supportsFamily:MTLGPUFamilyApple3] == YES )
        {
            m_maxTextureSize = 16384;
        }
        else
        {
            m_maxTextureSize = 8192;
        }
#endif

        LOGGER_INFO( "metal", "Metal max texture size: %u"
            , m_maxTextureSize
        );

        this->updateDepthStencilState_();

        for( const MetalRenderProgramPtr & program : m_deferredCompilePrograms )
        {
            program->setMetalDevice( m_device );

            const MetalRenderVertexShaderPtr & vertexShader = program->getMetalVertexShader();

            if( vertexShader != nullptr )
            {
                vertexShader->setMetalDevice( m_device );
            }

            const MetalRenderFragmentShaderPtr & fragmentShader = program->getMetalFragmentShader();

            if( fragmentShader != nullptr )
            {
                fragmentShader->setMetalDevice( m_device );
            }

            const MetalRenderVertexAttributePtr & vertexAttribute = program->getMetalVertexAttribute();

            if( vertexAttribute != nullptr )
            {
                vertexAttribute->setMetalDevice( m_device );
            }

            if( program->compile() == false )
            {
                return false;
            }

            program->setDeferredCompile( false );

            MetalRenderProgram * program_ptr = program.get();
            m_renderResourceHandlers.push_back( program_ptr );
        }

        m_deferredCompilePrograms.clear();

        m_renderWindowCreate = true;

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_CREATE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::destroyRenderWindow()
    {
        m_renderWindowCreate = false;

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_DESTROY );
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setScissor( const Viewport & _viewport )
    {
        mt::mat4f pm;
        mt::mul_m4_m4( &pm, m_projectionMatrix, m_viewMatrix );

        mt::vec2f b;
        mt::mul_v2_v2_m4( &b, _viewport.begin, pm );

        mt::vec2f e;
        mt::mul_v2_v2_m4( &e, _viewport.end, pm );

        mt::vec2f vs = m_viewport.size();

        float bx = (b.x + 1.f) * 0.5f * vs.x;
        float by = (1.f - (b.y + 1.f) * 0.5f) * vs.y;
        float ex = (e.x + 1.f) * 0.5f * vs.x;
        float ey = (1.f - (e.y + 1.f) * 0.5f) * vs.y;

        float left = bx;
        float top = by;
        float right = ex;
        float bottom = ey;

        if( left < 0.f ) left = 0.f;
        if( top < 0.f ) top = 0.f;
        if( right < left ) right = left;
        if( bottom < top ) bottom = top;

        if( right > vs.x ) right = vs.x;
        if( bottom > vs.y ) bottom = vs.y;

        float width = right - left;
        float height = bottom - top;

        if( m_renderEncoder != nil )
        {
            MTLScissorRect scissorRect;
            scissorRect.x = (NSUInteger)left;
            scissorRect.y = (NSUInteger)top;
            scissorRect.width = (NSUInteger)width;
            scissorRect.height = (NSUInteger)height;

            [m_renderEncoder setScissorRect:scissorRect];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::removeScissor()
    {
        if( m_renderEncoder != nil )
        {
            MTLScissorRect scissorRect;
            scissorRect.x = 0;
            scissorRect.y = 0;
            scissorRect.width = (NSUInteger)m_viewport.getWidth();
            scissorRect.height = (NSUInteger)m_viewport.getHeight();

            [m_renderEncoder setScissorRect:scissorRect];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;

        if( m_renderEncoder != nil )
        {
            MTLViewport viewport;
            viewport.originX = (double)m_viewport.begin.x;
            viewport.originY = (double)m_viewport.begin.y;
            viewport.width = (double)m_viewport.getWidth();
            viewport.height = (double)m_viewport.getHeight();
            viewport.znear = 0.0;
            viewport.zfar = 1.0;

            [m_renderEncoder setViewport:viewport];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setViewMatrix( const mt::mat4f & _viewMatrix )
    {
        m_viewMatrix = _viewMatrix;

        this->updatePMWMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        float Metal_PerfectPixelOffsetX = CONFIG_VALUE_FLOAT( "Metal", "PerfectPixelOffsetX", 0.f );
        float Metal_PerfectPixelOffsetY = CONFIG_VALUE_FLOAT( "Metal", "PerfectPixelOffsetY", 0.f );

        if( Metal_PerfectPixelOffsetX != 0.f || Metal_PerfectPixelOffsetY != 0.f )
        {
            float perfect_x = Metal_PerfectPixelOffsetX / (m_windowViewport.end.x - m_windowViewport.begin.x);
            float perfect_y = Metal_PerfectPixelOffsetY / (m_windowViewport.end.y - m_windowViewport.begin.y);

            mt::mat4f vmperfect;
            mt::make_translation_m4( &vmperfect, perfect_x, perfect_y, 0.f );

            mt::mul_m4_m4( &m_projectionMatrix, _projectionMatrix, vmperfect );
        }
        else
        {
            m_projectionMatrix = _projectionMatrix;
        }

        this->updatePMWMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture )
    {
        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _texture );
        // To Do
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setWorldMatrix( const mt::mat4f & _worldMatrix )
    {
        m_worldMatrix = _worldMatrix;

        this->updatePMWMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr MetalRenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        MetalRenderVertexBufferPtr buffer = m_factoryRenderVertexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create render vertex buffer (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        buffer->setMetalDevice( m_device );

        if( buffer->initialize( _vertexSize, _bufferType ) == false )
        {
            LOGGER_ERROR( "invalid initialize vertex buffer [%u] type [%u]"
                , _vertexSize
                , _bufferType
                );

            return nullptr;
        }

        MetalRenderVertexBuffer * buffer_ptr = buffer.get();
        m_renderResourceHandlers.push_back( buffer_ptr );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        m_currentVertexBuffer = _vertexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr MetalRenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        MetalRenderIndexBufferPtr buffer = m_factoryRenderIndexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create render index buffer (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        buffer->setMetalDevice( m_device );

        if( buffer->initialize( _indexSize, _bufferType ) == false )
        {
            LOGGER_ERROR( "invalid initialize index buffer (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        MetalRenderIndexBuffer * buffer_ptr = buffer.get();
        m_renderResourceHandlers.push_back( buffer_ptr );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        m_currentIndexBuffer = _indexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr MetalRenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentInterfacePtr & _doc )
    {
        MetalRenderVertexAttributePtr vertexAttribute = m_factoryRenderVertexAttribute->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute, "invalid create vertex attribute '%s'"
            , _name.c_str()
        );

        vertexAttribute->setMetalDevice( m_device );

        if( vertexAttribute->initialize( _name, _elementSize ) == false )
        {
            LOGGER_ERROR( "invalid initialize vertex attribute '%s' (doc: %s)"
                , _name.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr MetalRenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        MetalRenderFragmentShaderPtr fragmentShader = m_factoryRenderFragmentShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader, "invalid create shader '%s'"
            , _name.c_str()
        );

        fragmentShader->setMetalDevice( m_device );

        if( fragmentShader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s' (doc: %s)"
                , _name.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr MetalRenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        MetalRenderVertexShaderPtr vertexShader = m_factoryRenderVertexShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexShader, "invalid create shader '%s'"
            , _name.c_str()
        );

        vertexShader->setMetalDevice( m_device );

        if( vertexShader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s' (doc: %s)"
                , _name.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr MetalRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertexShader, const RenderFragmentShaderInterfacePtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc )
    {
        MetalRenderProgramPtr program = m_factoryRenderProgram->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( program, "invalid create program '%s'"
            , _name.c_str()
        );

        program->setMetalDevice( m_device );

        if( program->initialize( _name, _vertexShader, _fragmentShader, _vertexAttribute, _samplerCount ) == false )
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

            MetalRenderProgram * program_ptr = program.get();
            m_renderResourceHandlers.push_back( program_ptr );
        }
        else
        {
            program->setDeferredCompile( true );

            m_deferredCompilePrograms.push_back( program );
        }

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        m_currentProgram = _program;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr MetalRenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc )
    {
        MetalRenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( variable, "invalid create program variable" );

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program variable" );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _programVariable )
    {
        MENGINE_UNUSED( _program );

        m_currentProgramVariable = _programVariable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc )
    {
        MENGINE_UNUSED( _stageCache );

        MENGINE_ASSERTION_FATAL( m_currentIndexBuffer != nullptr, "index buffer not set" );
        MENGINE_ASSERTION_FATAL( m_currentVertexBuffer != nullptr, "vertex buffer not set" );
        MENGINE_ASSERTION_FATAL( m_currentProgram != nullptr, "program not set" );
        MENGINE_ASSERTION_FATAL( m_renderEncoder != nil, "render encoder not set" );

        MetalBlendStateKey blendKey;
        blendKey.alphaBlend = m_alphaBlend;
        blendKey.blendSrc = m_blendSrc;
        blendKey.blendDst = m_blendDst;
        blendKey.blendOp = m_blendOp;
        blendKey.blendSeparateSrc = m_blendSeparateSrc;
        blendKey.blendSeparateDst = m_blendSeparateDst;
        blendKey.blendSeparateOp = m_blendSeparateOp;
        blendKey.blendSeparate = m_blendSeparate;
        blendKey.colorMaskR = m_colorMaskR;
        blendKey.colorMaskG = m_colorMaskG;
        blendKey.colorMaskB = m_colorMaskB;
        blendKey.colorMaskA = m_colorMaskA;

        id<MTLRenderPipelineState> pipelineState = m_currentProgram->getOrCreatePipelineState( blendKey );

        if( pipelineState == nil )
        {
            return;
        }

        [m_renderEncoder setRenderPipelineState:pipelineState];

        m_currentProgram->bindMatrix( m_renderEncoder, m_worldMatrix, m_viewMatrix, m_projectionMatrix, m_totalWVPMatrix );

        if( m_currentProgramVariable != nullptr )
        {
            m_currentProgramVariable->apply( m_renderEncoder );
        }

        for( uint32_t stageId = 0; stageId != MENGINE_MAX_TEXTURE_STAGES; ++stageId )
        {
            const TextureStage & textureStage = m_textureStage[stageId];

            RenderImageInterface * texture = textureStage.texture;

            if( texture == nullptr )
            {
                continue;
            }

            MetalRenderImageExtensionInterface * extension = texture->getUnknown();
            extension->bind( stageId );

            id<MTLTexture> mtlTexture = extension->getMetalTexture();

            if( mtlTexture != nil )
            {
                [m_renderEncoder setFragmentTexture:mtlTexture atIndex:stageId];

                MTLSamplerDescriptor * samplerDesc = [[MTLSamplerDescriptor alloc] init];
                samplerDesc.minFilter = Helper::toMTLMinFilter( textureStage.minFilter );
                samplerDesc.magFilter = Helper::toMTLMagFilter( textureStage.magFilter );
                samplerDesc.mipFilter = Helper::toMTLMipFilter( textureStage.mipFilter );
                samplerDesc.sAddressMode = Helper::toMTLAddressMode( textureStage.wrapU );
                samplerDesc.tAddressMode = Helper::toMTLAddressMode( textureStage.wrapV );

                id<MTLSamplerState> samplerState = [m_device newSamplerStateWithDescriptor:samplerDesc];
                [m_renderEncoder setFragmentSamplerState:samplerState atIndex:stageId];
            }
        }

        id<MTLBuffer> vertexBuffer = m_currentVertexBuffer->getMetalBuffer();
        [m_renderEncoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

        [m_renderEncoder setDepthStencilState:m_depthStencilState];

        MTLCullMode mtlCullMode = Helper::toMTLCullMode( m_cullMode );
        [m_renderEncoder setCullMode:mtlCullMode];

        [m_renderEncoder setFrontFacingWinding:MTLWindingClockwise];

        id<MTLBuffer> indexBuffer = m_currentIndexBuffer->getMetalBuffer();
        MTLIndexType indexType = Helper::toMTLIndexType( sizeof( RenderIndex ) );
        MTLPrimitiveType primitiveType = Helper::toMTLPrimitiveType( _desc.primitiveType );

        NSUInteger indexBufferOffset = _desc.startIndex * sizeof( RenderIndex );

        [m_renderEncoder drawIndexedPrimitives:primitiveType
                                    indexCount:_desc.indexCount
                                     indexType:indexType
                                   indexBuffer:indexBuffer
                             indexBufferOffset:indexBufferOffset];

        for( uint32_t stageId = 0; stageId != MENGINE_MAX_TEXTURE_STAGES; ++stageId )
        {
            TextureStage & textureStage = m_textureStage[stageId];

            RenderImageInterface * texture = textureStage.texture;

            if( texture == nullptr )
            {
                break;
            }

            MetalRenderImageExtensionInterface * extension = texture->getUnknown();
            extension->unbind( stageId );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stageId, const RenderImageInterfacePtr & _texture )
    {
        MENGINE_UNUSED( _program );

        MENGINE_ASSERTION_FATAL( _stageId < MENGINE_MAX_TEXTURE_STAGES, "invalid stage id %u"
            , _stageId
        );

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
    void MetalRenderSystem::setTextureAddressing( uint32_t _stageId, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        MENGINE_ASSERTION_FATAL( _stageId < MENGINE_MAX_TEXTURE_STAGES, "invalid stage id %u"
            , _stageId
        );

        m_textureStage[_stageId].wrapU = _modeU;
        m_textureStage[_stageId].wrapV = _modeV;
        m_textureStage[_stageId].border = _border;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTextureFactor( uint32_t _color )
    {
        MENGINE_UNUSED( _color );

        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate )
    {
        m_blendSrc = _src;
        m_blendDst = _dst;
        m_blendOp = _op;
        m_blendSeparateSrc = _separateSrc;
        m_blendSeparateDst = _separateDst;
        m_blendSeparateOp = _separateOp;
        m_blendSeparate = _separate;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setCullMode( ECullMode _mode )
    {
        m_cullMode = _mode;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderSystem::getAvailableTextureMemory() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        m_depthTest = _depthTest;

        this->updateDepthStencilState_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        m_depthMask = _depthWrite;

        this->updateDepthStencilState_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        m_depthFunction = _depthFunction;

        this->updateDepthStencilState_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setFillMode( EFillMode _mode )
    {
        m_fillMode = _mode;

        if( m_renderEncoder != nil )
        {
            MTLTriangleFillMode mtlFillMode = Helper::toMTLFillMode( _mode );
            [m_renderEncoder setTriangleFillMode:mtlFillMode];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        m_colorMaskR = _r;
        m_colorMaskG = _g;
        m_colorMaskB = _b;
        m_colorMaskA = _a;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        m_alphaBlend = _alphaBlend;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTextureStageFilter( uint32_t _stageId, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        MENGINE_ASSERTION_FATAL( _stageId < MENGINE_MAX_TEXTURE_STAGES, "invalid stage id %u"
            , _stageId
        );

        TextureStage & tStage = m_textureStage[_stageId];

        tStage.minFilter = _minification;
        tStage.magFilter = _magnification;
        tStage.mipFilter = _mipmap;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::findFormatFromChannels_( EPixelFormat _format, EPixelFormat * const _hwFormat ) const
    {
        switch( _format )
        {
        case PF_R8G8B8:
            {
                *_hwFormat = PF_X8R8G8B8;
            }break;
        default:
            {
                *_hwFormat = _format;
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MetalRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        EPixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, &hwFormat );

        uint32_t hwWidth = Helper::getTexturePow2( _width );
        uint32_t hwHeight = Helper::getTexturePow2( _height );
        uint32_t maxTextureSize = this->getMaxTextureSize();

        if( maxTextureSize > 0 && (hwWidth > maxTextureSize || hwHeight > maxTextureSize) )
        {
            LOGGER_ERROR( "invalid texture size exceeds maximum: size %u:%u hwSize %u:%u maxSize %u PF %u"
                , _width
                , _height
                , hwWidth
                , hwHeight
                , maxTextureSize
                , hwFormat
            );

            return nullptr;
        }

        MetalRenderImagePtr image = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( image, "invalid create" );

        image->setMetalDevice( m_device );

        if( image->initialize( _mipmaps, _width, _height, hwFormat ) == false )
        {
            LOGGER_ERROR( "invalid initialize" );

            return nullptr;
        }

        MetalRenderImage * image_ptr = image.get();
        m_renderResourceHandlers.push_back( image_ptr );

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::beginScene()
    {
        if( m_currentDrawableTexture == nil )
        {
            return false;
        }

        m_commandBuffer = [m_commandQueue commandBuffer];

        if( m_commandBuffer == nil )
        {
            return false;
        }

        NSUInteger drawableWidth = [m_currentDrawableTexture width];
        NSUInteger drawableHeight = [m_currentDrawableTexture height];

        if( m_depthStencilTexture == nil || m_depthStencilWidth != (uint32_t)drawableWidth || m_depthStencilHeight != (uint32_t)drawableHeight )
        {
            this->createDepthStencilTexture_( (uint32_t)drawableWidth, (uint32_t)drawableHeight );
        }

        MTLRenderPassDescriptor * renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        renderPassDescriptor.colorAttachments[0].texture = m_currentDrawableTexture;
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake( m_clearColor.getR(), m_clearColor.getG(), m_clearColor.getB(), m_clearColor.getA() );

        if( m_depthStencilTexture != nil )
        {
            renderPassDescriptor.depthAttachment.texture = m_depthStencilTexture;
            renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
            renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionDontCare;
            renderPassDescriptor.depthAttachment.clearDepth = m_clearDepth;

            renderPassDescriptor.stencilAttachment.texture = m_depthStencilTexture;
            renderPassDescriptor.stencilAttachment.loadAction = MTLLoadActionClear;
            renderPassDescriptor.stencilAttachment.storeAction = MTLStoreActionDontCare;
            renderPassDescriptor.stencilAttachment.clearStencil = (uint32_t)m_clearStencil;
        }

        m_renderEncoder = [m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

        if( m_renderEncoder == nil )
        {
            m_commandBuffer = nil;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::endScene()
    {
        if( m_renderEncoder != nil )
        {
            [m_renderEncoder endEncoding];
            m_renderEncoder = nil;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::swapBuffers()
    {
        if( m_commandBuffer != nil )
        {
            if( m_currentDrawable != nil )
            {
                [m_commandBuffer presentDrawable:m_currentDrawable];
            }

            [m_commandBuffer commit];
            m_commandBuffer = nil;
        }

        m_currentDrawable = nil;
        m_currentDrawableTexture = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil )
    {
        if( (_frameBufferTypes & FBT_COLOR) != 0 )
        {
            m_clearColor = _color;
        }

        if( (_frameBufferTypes & FBT_DEPTH) != 0 )
        {
            m_clearDepth = _depth;
        }

        if( (_frameBufferTypes & FBT_STENCIL) != 0 )
        {
            m_clearStencil = _stencil;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_viewport = Viewport( 0.f, 0.f, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderSystem::getMaxCombinedTextureImageUnits() const
    {
        return m_maxCombinedTextureImageUnits;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderSystem::getMaxTextureSize() const
    {
        return m_maxTextureSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onWindowMovedOrResized()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onWindowClose()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onDeviceLostPrepare()
    {
        MENGINE_ASSERTION_FATAL( m_renderDeviceLost == false, "already device lost" );

        m_renderDeviceLost = true;

        for( MetalRenderResourceHandler * resource : m_renderResourceHandlers )
        {
            resource->onRenderReset();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::onDeviceLostRestore()
    {
        if( m_renderDeviceLost == false )
        {
            return true;
        }

        m_renderDeviceLost = false;

        for( MetalRenderResourceHandler * resource : m_renderResourceHandlers )
        {
            if( resource->onRenderRestore() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setVSync( bool _vSync )
    {
        MENGINE_UNUSED( _vSync );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MetalRenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        EPixelFormat hwFormat = PF_UNKNOWN;
        this->findFormatFromChannels_( _format, &hwFormat );

        MetalRenderTargetTexturePtr renderTarget = m_factoryRenderTargetTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTarget, "invalid create render target texture" );

        renderTarget->setMetalDevice( m_device );
        renderTarget->setRenderEncoderContext( &m_commandBuffer, &m_renderEncoder, &m_currentDrawableTexture, &m_depthStencilTexture );

        if( renderTarget->initialize( _width, _height, hwFormat ) == false )
        {
            LOGGER_ERROR( "invalid initialize" );

            return nullptr;
        }

        MetalRenderTargetTexture * renderTarget_ptr = renderTarget.get();
        m_renderResourceHandlers.push_back( renderTarget_ptr );

        return renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MetalRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _width );
        MENGINE_UNUSED( _height );
        MENGINE_UNUSED( _format );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MetalRenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentInterfacePtr & _doc )
    {
        MetalRenderTargetTexturePtr targetTexture = stdex::intrusive_static_cast<MetalRenderTargetTexturePtr>(_renderTarget);

        MetalRenderImageTargetPtr imageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageTarget, "invalid create render image target" );

        if( imageTarget->initialize( targetTexture ) == false )
        {
            LOGGER_ERROR( "invalid initialize" );

            return nullptr;
        }

        MetalRenderImageTarget * imageTarget_ptr = imageTarget.get();
        m_renderResourceHandlers.push_back( imageTarget_ptr );

        return imageTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialStageCacheInterfacePtr MetalRenderSystem::createRenderMaterialStageCache( const RenderMaterialStage * _stage, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _doc );

        //Empty

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLDevice> MetalRenderSystem::getMetalDevice() const
    {
        return m_device;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setCurrentDrawable( id<MTLDrawable> _drawable, id<MTLTexture> _drawableTexture )
    {
        m_currentDrawable = _drawable;
        m_currentDrawableTexture = _drawableTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderVertexBufferDestroy_( MetalRenderVertexBuffer * _vertexBuffer )
    {
        _vertexBuffer->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderIndexBufferDestroy_( MetalRenderIndexBuffer * _indexBuffer )
    {
        _indexBuffer->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderImageDestroy_( MetalRenderImage * _image )
    {
        _image->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderImageTargetDestroy_( MetalRenderImageTarget * _imageTarget )
    {
        _imageTarget->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderTargetTextureDestroy_( MetalRenderTargetTexture * _targetTexture )
    {
        _targetTexture->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderVertexShaderDestroy_( MetalRenderVertexShader * _vertexShader )
    {
        _vertexShader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderFragmentShaderDestroy_( MetalRenderFragmentShader * _fragmentShader )
    {
        _fragmentShader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onRenderProgramDestroy_( MetalRenderProgram * _program )
    {
        if( m_renderDeviceLost == false )
        {
            if( _program->getDeferredCompile() == false )
            {
                _program->release();
            }
        }

        _program->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::updatePMWMatrix_()
    {
        m_totalWVPMatrix = m_worldMatrix * m_viewMatrix * m_projectionMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::updateDepthStencilState_()
    {
        if( m_device == nil )
        {
            return;
        }

        MTLDepthStencilDescriptor * depthDesc = [[MTLDepthStencilDescriptor alloc] init];

        if( m_depthTest == true )
        {
            depthDesc.depthCompareFunction = Helper::toMTLCmpFunc( m_depthFunction );
        }
        else
        {
            depthDesc.depthCompareFunction = MTLCompareFunctionAlways;
        }

        depthDesc.depthWriteEnabled = m_depthMask ? YES : NO;

        m_depthStencilState = [m_device newDepthStencilStateWithDescriptor:depthDesc];
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::createDepthStencilTexture_( uint32_t _width, uint32_t _height )
    {
        if( _width == 0 || _height == 0 )
        {
            return;
        }

        MTLTextureDescriptor * dsDesc = [[MTLTextureDescriptor alloc] init];
        dsDesc.textureType = MTLTextureType2D;
        dsDesc.pixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        dsDesc.width = _width;
        dsDesc.height = _height;
        dsDesc.usage = MTLTextureUsageRenderTarget;
        dsDesc.storageMode = MTLStorageModePrivate;

        id<MTLTexture> dsTexture = [m_device newTextureWithDescriptor:dsDesc];

        if( dsTexture == nil )
        {
            LOGGER_ERROR( "failed to create depth/stencil texture %u:%u"
                , _width
                , _height
            );

            return;
        }

        m_depthStencilTexture = dsTexture;
        m_depthStencilWidth = _width;
        m_depthStencilHeight = _height;
    }
    //////////////////////////////////////////////////////////////////////////
}
