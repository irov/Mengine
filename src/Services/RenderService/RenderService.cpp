#include "RenderService.h"

#include "Interface/CodecInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/WatchdogServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "BatchRenderPipeline.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/RenderHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/StatisticHelper.h"

#include "Config/Algorithm.h"
#include "Config/StdMath.h"

#include "stdex/memorycopy.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderService, Mengine::RenderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderService::RenderService()
        : m_renderSystem( nullptr )
        , m_windowCreated( false )
        , m_vsync( false )
        , m_fullscreen( false )
        , m_maxVertexCount( 0 )
        , m_maxIndexCount( 0 )
        , m_currentRenderTextureStage( 0 )
        , m_currentMaterialId( 0 )
        , m_currentRenderMaterialStage( nullptr )
        , m_currentBlendSrc( BF_ONE )
        , m_currentBlendDst( BF_ZERO )
        , m_currentBlendOp( BOP_ADD )
        , m_currentSeparateAlphaBlendSrc( BF_ONE )
        , m_currentSeparateAlphaBlendDst( BF_ZERO )
        , m_currentSeparateAlphaBlendOp( BOP_ADD )
        , m_currentSeparateAlphaBlendEnable( false )
        , m_currentAlphaBlendEnable( false )
        , m_currentDepthBufferTestEnable( false )
        , m_currentDepthBufferWriteEnable( false )
    {
        Algorithm::fill_n( m_currentTexturesID, MENGINE_MAX_TEXTURE_STAGES, 0u );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderService::~RenderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::_initializeService()
    {
        m_maxVertexCount = CONFIG_VALUE( "Engine", "RenderMaxVertexCount", 32000U );
        m_maxIndexCount = CONFIG_VALUE( "Engine", "RenderMaxIndexCount", 48000U );

        STATISTIC_RESET_INTEGER( "DrawIndexPrimitives" );
        STATISTIC_RESET_INTEGER( "RenderFrame" );
        STATISTIC_RESET_DOUBLE( "RenderFillrate" );
        STATISTIC_RESET_INTEGER( "RenderObjects" );
        STATISTIC_RESET_INTEGER( "RenderTriangles" );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "RenderPipeline" ), STRINGIZE_STRING_LOCAL( "Batch" ), Helper::makeDefaultPrototypeGenerator<BatchRenderPipeline, 8>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        m_factoryRenderBatch = Helper::makeFactoryPool<RenderBatch, 16>( MENGINE_DOCUMENT_FACTORABLE );

        m_renderSystem = RENDER_SYSTEM();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::_finalizeService()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "RenderPipeline" ), STRINGIZE_STRING_LOCAL( "Batch" ), nullptr );

        m_renderBatches.clear();

        for( const RenderBatchPtr & renderBatch : m_cacheRenderBatches )
        {
            renderBatch->finalize();
        }

        m_cacheRenderBatches.clear();

        m_debugRenderVertices.clear();
        m_debugRenderIndices.clear();

        m_nullTexture = nullptr;
        m_whiteTexture = nullptr;

        m_currentRenderVertexBuffer = nullptr;
        m_currentRenderIndexBuffer = nullptr;
        m_currentRenderProgramVariable = nullptr;
        m_currentRenderProgram = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderBatch );

        m_factoryRenderBatch = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::_stopService()
    {
        if( m_windowCreated == false )
        {
            return;
        }

        this->restoreRenderSystemStates_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::createRenderWindow( const Resolution & _windowResolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _vsync, uint32_t _bits, bool _fullscreen, int32_t _FSAAType, int32_t _FSAAQuality )
    {
        m_windowResolution = _windowResolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;
        m_vsync = _vsync;
        m_fullscreen = _fullscreen;

        LOGGER_INFO( "render", "window resolution [%u, %u]\ncontent resolution [%u, %u]\nvsync %u\nfullscreen %u"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , m_contentResolution.getWidth()
            , m_contentResolution.getHeight()
            , m_vsync
            , m_fullscreen
        );

        bool Engine_RenderEnableAutoDepthStencil = CONFIG_VALUE( "Engine", "RenderEnableAutoDepthStencil", false );
        uint32_t Engine_RenderMultiSampleCount = CONFIG_VALUE( "Engine", "RenderMultiSampleCount", 2U );

        LOGGER_MESSAGE( "Render auto depth stencil: %s"
            , Engine_RenderEnableAutoDepthStencil == true ? "true" : "false"
        );

        LOGGER_MESSAGE( "Render multi sample count: %u"
            , Engine_RenderMultiSampleCount
        );

        RenderWindowDesc desc;
        desc.resolution = m_windowResolution;
        desc.bits = _bits;
        desc.fullscreen = _fullscreen;
        desc.depth = Engine_RenderEnableAutoDepthStencil;
        desc.waitForVSync = m_vsync;
        desc.FSAAType = _FSAAType;
        desc.FSAAQuality = _FSAAQuality;
        desc.MultiSampleCount = Engine_RenderMultiSampleCount;

        if( m_renderSystem->createRenderWindow( &desc ) == false )
        {
            LOGGER_ERROR( "invalid create render window" );

            return false;
        }

        this->clearFrameBuffer_();

        m_windowCreated = true;

        if( this->createNullTexture_() == false )
        {
            return false;
        }

        if( this->createWhitePixelTexture_() == false )
        {
            return false;
        }

        m_defaultRenderTextureStage.mipmap = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMipmap();

        m_defaultRenderTextureStage.minification = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMinification();

        m_defaultRenderTextureStage.magnification = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMagnification();

        this->restoreRenderSystemStates_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::destroyRenderWindow()
    {
        m_nullTexture = nullptr;
        m_whiteTexture = nullptr;

        m_currentRenderVertexBuffer = nullptr;
        m_currentRenderIndexBuffer = nullptr;
        m_currentRenderProgramVariable = nullptr;
        m_currentRenderProgram = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::createNullTexture_()
    {
        LOGGER_INFO( "render", "create null texture" );

        uint32_t mipmaps = 1;
        uint32_t width = 2;
        uint32_t height = 2;
        EPixelFormat format = PF_A8R8G8B8;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( mipmaps, width, height, format, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid create null texture [%u:%u]"
            , width
            , height
        );

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = width;
        rect.bottom = height;

        RenderImageLockedInterfacePtr locked = image->lock( 0, rect, false );

        size_t pitch = 0;
        void * textureData = locked->getBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( textureData, "invalid lock null texture [%u:%u]"
            , width
            , height
        );

        uint8_t * buffer_textureData = static_cast<uint8_t *>(textureData);

        buffer_textureData[0] = 0xFF;
        buffer_textureData[1] = 0x00;
        buffer_textureData[2] = 0x00;
        buffer_textureData[3] = 0xFF;

        buffer_textureData[4] = 0x00;
        buffer_textureData[5] = 0xFF;
        buffer_textureData[6] = 0x00;
        buffer_textureData[7] = 0xFF;

        buffer_textureData += pitch;

        buffer_textureData[0] = 0x00;
        buffer_textureData[1] = 0x00;
        buffer_textureData[2] = 0xFF;
        buffer_textureData[3] = 0xFF;

        buffer_textureData[4] = 0x00;
        buffer_textureData[5] = 0x00;
        buffer_textureData[6] = 0x00;
        buffer_textureData[7] = 0xFF;

        image->unlock( locked, 0, true );

        m_nullTexture = texture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::createWhitePixelTexture_()
    {
        LOGGER_INFO( "render", "create white pixel texture" );

        uint32_t mipmaps = 1;
        uint32_t width = 2;
        uint32_t height = 2;
        EPixelFormat format = PF_A8R8G8B8;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( mipmaps, width, height, format, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid create null texture [%u:%u]"
            , width
            , height
        );

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = width;
        rect.bottom = height;

        RenderImageLockedInterfacePtr locked = image->lock( 0, rect, false );

        size_t pitch = 0;
        void * textureData = locked->getBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( textureData, "invalid lock null texture [%u:%u]"
            , width
            , height
        );

        uint8_t * buffer_textureData = static_cast<uint8_t *>(textureData);

        buffer_textureData[0] = 0xFF;
        buffer_textureData[1] = 0xFF;
        buffer_textureData[2] = 0xFF;
        buffer_textureData[3] = 0xFF;

        buffer_textureData[4] = 0xFF;
        buffer_textureData[5] = 0xFF;
        buffer_textureData[6] = 0xFF;
        buffer_textureData[7] = 0xFF;

        buffer_textureData += pitch;

        buffer_textureData[0] = 0xFF;
        buffer_textureData[1] = 0xFF;
        buffer_textureData[2] = 0xFF;
        buffer_textureData[3] = 0xFF;

        buffer_textureData[4] = 0xFF;
        buffer_textureData[5] = 0xFF;
        buffer_textureData[6] = 0xFF;
        buffer_textureData[7] = 0xFF;

        image->unlock( locked, 0, true );

        m_whiteTexture = texture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureInterfacePtr & RenderService::getNullTexture() const
    {
        return m_nullTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureInterfacePtr & RenderService::getWhiteTexture() const
    {
        return m_whiteTexture;
    }
    ////////////////////////////////////////////////////////////////////////////
    void RenderService::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;

        m_fullscreen = _fullscreen;

        LOGGER_INFO( "render", "window resolution [%u, %u]\ncontent resolution [%u, %u]\nrender viewport [%f %f %f %f]\nfullscreen %u"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , m_contentResolution.getWidth()
            , m_contentResolution.getHeight()
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.end.x
            , m_renderViewport.end.y
            , m_fullscreen
        );

        if( m_windowCreated == false )
        {
            return;
        }

        m_renderSystem->changeWindowMode( m_windowResolution, m_fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::onWindowClose()
    {
        if( m_windowCreated == false )
        {
            return;
        }

        m_renderSystem->onWindowClose();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::onDeviceLostPrepare()
    {
        if( m_windowCreated == false )
        {
            return;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );

        m_renderBatches.clear();

        for( const RenderBatchPtr & batch : m_cacheRenderBatches )
        {
            batch->deviceLostPrepare();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::onDeviceLostRestore()
    {
        if( m_windowCreated == false )
        {
            return true;
        }

        for( const RenderBatchPtr & batch : m_cacheRenderBatches )
        {
            batch->deviceLostRestore();
        }

        this->restoreRenderSystemStates_();

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::clearFrameBuffer_()
    {
        bool Engine_RenderFrameClear = CONFIG_VALUE( "Engine", "RenderFrameClear", true );

        if( Engine_RenderFrameClear == true )
        {
            uint8_t Engine_RenderFrameClearColorR = CONFIG_VALUET( "Engine", "RenderFrameClearColorR", 0U, uint8_t );
            uint8_t Engine_RenderFrameClearColorG = CONFIG_VALUET( "Engine", "RenderFrameClearColorG", 0U, uint8_t );
            uint8_t Engine_RenderFrameClearColorB = CONFIG_VALUET( "Engine", "RenderFrameClearColorB", 0U, uint8_t );

            Color argb = Helper::makeColor8( Engine_RenderFrameClearColorR, Engine_RenderFrameClearColorG, Engine_RenderFrameClearColorB, 255U );

            uint32_t frameBufferTypes = FBT_COLOR;

            bool Engine_RenderEnableAutoDepthStencil = CONFIG_VALUE( "Engine", "RenderEnableAutoDepthStencil", false );

            if( Engine_RenderEnableAutoDepthStencil == true )
            {
                frameBufferTypes |= FBT_DEPTH;
            }

            m_renderSystem->clearFrameBuffer( frameBufferTypes, argb, 1.f, 0x00000000 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::beginScene( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        MENGINE_PROFILER_CATEGORY();

        _renderPipeline->prepare();

        if( m_renderSystem->beginScene() == false )
        {
            return false;
        }

        this->restoreRenderSystemStates_();

        this->clearFrameBuffer_();

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_SCENE_BEGIN, _renderPipeline );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::endScene( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        MENGINE_PROFILER_CATEGORY();

        _renderPipeline->flush();

        this->flushRender_( _renderPipeline );

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_SCENE_END, _renderPipeline );

        m_renderSystem->endScene();

        STATISTIC_ADD_INTEGER( "RenderFrame", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::swapBuffers()
    {
        MENGINE_PROFILER_CATEGORY();

        m_renderSystem->swapBuffers();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::isWindowCreated() const
    {
        return m_windowCreated;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::updateStage_( const RenderMaterialStage * _stage )
    {
        if( m_currentRenderMaterialStage == _stage )
        {
            return;
        }

        m_currentRenderMaterialStage = _stage;

        for( uint32_t stageId = 0; stageId != m_currentRenderTextureStage; ++stageId )
        {
            RenderTextureStage & current_texture_stage = m_renderTextureStages[stageId];
            const RenderTextureStage & texture_stage = m_currentRenderMaterialStage->textureStages[stageId];

            if( current_texture_stage.addressU != texture_stage.addressU
                || current_texture_stage.addressV != texture_stage.addressV
                || current_texture_stage.addressBorder != texture_stage.addressBorder )
            {
                current_texture_stage.addressU = texture_stage.addressU;
                current_texture_stage.addressV = texture_stage.addressV;
                current_texture_stage.addressBorder = texture_stage.addressBorder;

                m_renderSystem->setTextureAddressing( stageId
                    , current_texture_stage.addressU
                    , current_texture_stage.addressV
                    , current_texture_stage.addressBorder
                );
            }

            if( current_texture_stage.mipmap != texture_stage.mipmap ||
                current_texture_stage.magnification != texture_stage.magnification ||
                current_texture_stage.minification != texture_stage.minification )
            {
                current_texture_stage.mipmap = texture_stage.mipmap;
                current_texture_stage.magnification = texture_stage.magnification;
                current_texture_stage.minification = texture_stage.minification;

                m_renderSystem->setTextureStageFilter( stageId
                    , current_texture_stage.minification
                    , current_texture_stage.mipmap
                    , current_texture_stage.magnification
                );
            }
        }

        if( m_currentAlphaBlendEnable != m_currentRenderMaterialStage->alphaBlendEnable )
        {
            m_currentAlphaBlendEnable = m_currentRenderMaterialStage->alphaBlendEnable;

            m_renderSystem->setAlphaBlendEnable( m_currentAlphaBlendEnable );
        }

        if( m_currentDepthBufferTestEnable != m_currentRenderMaterialStage->depthBufferTestEnable )
        {
            m_currentDepthBufferTestEnable = m_currentRenderMaterialStage->depthBufferTestEnable;

            m_renderSystem->setDepthBufferTestEnable( m_currentDepthBufferTestEnable );
        }

        if( m_currentDepthBufferWriteEnable != m_currentRenderMaterialStage->depthBufferWriteEnable )
        {
            m_currentDepthBufferWriteEnable = m_currentRenderMaterialStage->depthBufferWriteEnable;

            m_renderSystem->setDepthBufferWriteEnable( m_currentDepthBufferWriteEnable );
        }

        if( m_currentBlendSrc != m_currentRenderMaterialStage->blendSrc ||
            m_currentBlendDst != m_currentRenderMaterialStage->blendDst ||
            m_currentBlendOp != m_currentRenderMaterialStage->blendOp ||
            m_currentSeparateAlphaBlendEnable != m_currentRenderMaterialStage->separateAlphaBlendEnable ||
            m_currentSeparateAlphaBlendSrc != m_currentRenderMaterialStage->separateAlphaBlendSrc ||
            m_currentSeparateAlphaBlendDst != m_currentRenderMaterialStage->separateAlphaBlendDst ||
            m_currentSeparateAlphaBlendOp != m_currentRenderMaterialStage->separateAlphaBlendOp )
        {
            m_currentBlendSrc = m_currentRenderMaterialStage->blendSrc;
            m_currentBlendDst = m_currentRenderMaterialStage->blendDst;
            m_currentBlendOp = m_currentRenderMaterialStage->blendOp;

            m_currentSeparateAlphaBlendEnable = m_currentRenderMaterialStage->separateAlphaBlendEnable;

            m_currentSeparateAlphaBlendSrc = m_currentRenderMaterialStage->separateAlphaBlendSrc;
            m_currentSeparateAlphaBlendDst = m_currentRenderMaterialStage->separateAlphaBlendDst;
            m_currentSeparateAlphaBlendOp = m_currentRenderMaterialStage->separateAlphaBlendOp;

            m_renderSystem->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp, m_currentSeparateAlphaBlendSrc, m_currentSeparateAlphaBlendDst, m_currentSeparateAlphaBlendOp, m_currentSeparateAlphaBlendEnable );
        }

        if( m_currentRenderProgram != m_currentRenderMaterialStage->program )
        {
            m_currentRenderProgram = m_currentRenderMaterialStage->program;

            m_renderSystem->setProgram( m_currentRenderProgram );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::updateTexture_( uint32_t _stageId, const RenderTextureInterfacePtr & _texture )
    {
        uint32_t texture_id = _texture->getId();
        uint32_t current_texture_id = m_currentTexturesID[_stageId];

        if( texture_id != current_texture_id || current_texture_id == 0 )
        {
            m_currentTexturesID[_stageId] = texture_id;

            const RenderImageInterfacePtr & image = _texture->getImage();

            m_renderSystem->setTexture( m_currentRenderProgram, _stageId, image );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::updateMaterial_( const RenderMaterialInterface * _material )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _material );

        uint32_t materialId = _material->getId();

        if( m_currentMaterialId == materialId )
        {
            return;
        }

        const RenderMaterialStage * stage = _material->getStage();

        this->updateStage_( stage );

        m_currentMaterialId = materialId;

        uint32_t textureCount = _material->getTextureCount();

        if( m_currentRenderTextureStage > textureCount )
        {
            for( uint32_t stageId = textureCount; stageId != m_currentRenderTextureStage; ++stageId )
            {
                this->restoreTextureStage_( stageId );
            }
        }

        m_currentRenderTextureStage = textureCount;

        for( uint32_t stageId = 0; stageId != m_currentRenderTextureStage; ++stageId )
        {
            const RenderTextureInterfacePtr & texture = _material->getTexture( stageId );

            if( texture == nullptr )
            {
                this->updateTexture_( stageId, m_nullTexture );
            }
            else
            {
                this->updateTexture_( stageId, texture );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::renderPrimitive_( const RenderPrimitive * _renderPrimitive )
    {
        RenderMaterialInterface * material = _renderPrimitive->material;

        if( _renderPrimitive->indexCount == 0 )
        {
            if( material != nullptr )
            {
                MENGINE_ASSERTION_FATAL( IntrusivePtrBase::intrusive_ptr_get_ref( material ) > 1, "not-cached material" );

                IntrusivePtrBase::intrusive_ptr_dec_ref( material );
            }

            return;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( material );

        this->updateMaterial_( material );

        m_renderSystem->updateProgram( m_currentRenderProgram );

        m_renderSystem->setProgramVariable( m_currentRenderProgram, m_currentRenderProgramVariable );

        EPrimitiveType primitiveType = material->getPrimitiveType();

        RenderIndexedPrimitiveDesc desc;
        desc.primitiveType = primitiveType;
        desc.baseVertexIndex = _renderPrimitive->baseVertexIndex;
        desc.minIndex = _renderPrimitive->minIndex;
        desc.vertexCount = _renderPrimitive->vertexCount;
        desc.startIndex = _renderPrimitive->startIndex;
        desc.indexCount = _renderPrimitive->indexCount;

        const RenderMaterialStage * materialStage = material->getStage();
        const RenderMaterialStageCacheInterfacePtr & stageCache = materialStage->stageCache;

        m_renderSystem->drawIndexedPrimitive( stageCache, desc );

        MENGINE_ASSERTION_FATAL( IntrusivePtrBase::intrusive_ptr_get_ref( material ) > 1, "not-cached material" );

        IntrusivePtrBase::intrusive_ptr_dec_ref( material );

        STATISTIC_ADD_INTEGER( "DrawIndexPrimitives", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::restoreTextureStage_( uint32_t _stage )
    {
        m_renderTextureStages[_stage] = m_defaultRenderTextureStage;

        m_currentTexturesID[_stage] = 0;

        m_renderSystem->setTexture( m_currentRenderProgram, _stage, nullptr );

        m_renderSystem->setTextureAddressing( _stage
            , m_defaultRenderTextureStage.addressU
            , m_defaultRenderTextureStage.addressV
            , m_defaultRenderTextureStage.addressBorder
        );

        m_renderSystem->setTextureStageFilter( _stage
            , m_defaultRenderTextureStage.minification
            , m_defaultRenderTextureStage.mipmap
            , m_defaultRenderTextureStage.magnification
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::restoreRenderFrameStates_()
    {
        m_renderBatches.clear();

        for( const RenderBatchPtr & batch : m_cacheRenderBatches )
        {
            batch->restore();
        }

        m_debugRenderVertices.clear();
        m_debugRenderIndices.clear();

        STATISTIC_RESET_DOUBLE( "RenderFillrate" );
        STATISTIC_RESET_DOUBLE( "RenderObjects" );
        STATISTIC_RESET_DOUBLE( "RenderTriangles" );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::restoreRenderSystemStates_()
    {
        this->restoreRenderFrameStates_();

        mt::mat4f viewMatrix;
        mt::ident_m4( viewMatrix );

        m_renderSystem->setViewMatrix( viewMatrix );

        mt::mat4f projectionMatrix;
        mt::ident_m4( projectionMatrix );

        m_renderSystem->setProjectionMatrix( projectionMatrix );

        uint32_t width = m_windowResolution.getWidth();
        uint32_t height = m_windowResolution.getHeight();

        Viewport renderViewport;
        renderViewport.begin.x = 0.f;
        renderViewport.begin.y = 0.f;
        renderViewport.end.x = (float)width;
        renderViewport.end.y = (float)height;

        m_renderSystem->setViewport( renderViewport );

        Helper::clearRenderContext( &m_currentRenderContext );

        const mt::mat4f & worldMatrix = mt::mat4f::identity();

        m_renderSystem->setWorldMatrix( worldMatrix );

        m_renderSystem->removeScissor();

        m_currentMaterialId = 0;
        m_currentRenderTextureStage = 0;
        m_currentRenderMaterialStage = nullptr;

        m_currentBlendSrc = BF_ONE;
        m_currentBlendDst = BF_ZERO;
        m_currentBlendOp = BOP_ADD;

        m_currentSeparateAlphaBlendSrc = BF_ONE;
        m_currentSeparateAlphaBlendDst = BF_ZERO;
        m_currentSeparateAlphaBlendOp = BOP_ADD;

        m_currentSeparateAlphaBlendEnable = false;
        m_currentAlphaBlendEnable = false;
        m_currentDepthBufferTestEnable = false;
        m_currentDepthBufferWriteEnable = false;

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            this->restoreTextureStage_( i );
        }

        Algorithm::fill_n( m_currentTexturesID, MENGINE_MAX_TEXTURE_STAGES, 0 );

        m_currentRenderVertexBuffer = nullptr;
        m_currentRenderIndexBuffer = nullptr;
        m_currentRenderProgramVariable = nullptr;
        m_currentRenderProgram = nullptr;

        m_renderSystem->setVertexBuffer( m_currentRenderVertexBuffer );
        m_renderSystem->setIndexBuffer( m_currentRenderIndexBuffer );
        m_renderSystem->setProgram( m_currentRenderProgram );
        m_renderSystem->setCullMode( CM_CULL_NONE );
        //m_renderSystem->setFillMode( FM_SOLID );
        //m_renderSystem->setFillMode( FM_WIREFRAME );
        m_renderSystem->setDepthBufferTestEnable( m_currentDepthBufferTestEnable );
        m_renderSystem->setDepthBufferWriteEnable( m_currentDepthBufferWriteEnable );
        m_renderSystem->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
        m_renderSystem->setAlphaBlendEnable( m_currentAlphaBlendEnable );
        m_renderSystem->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp, m_currentSeparateAlphaBlendSrc, m_currentSeparateAlphaBlendDst, m_currentSeparateAlphaBlendOp, m_currentSeparateAlphaBlendEnable );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::calcRenderViewport_( const Viewport & _viewport, Viewport * const _renderViewport ) const
    {
        const Viewport & renderViewport = this->getRenderViewport();

        float renderWidth = renderViewport.getWidth();
        float renderHeight = renderViewport.getHeight();

        uint32_t contentWidth = m_contentResolution.getWidth();
        uint32_t contentHeight = m_contentResolution.getHeight();

        float scale_width = renderWidth / float( contentWidth );
        float scale_height = renderHeight / float( contentHeight );

        Viewport scaleViewport;
        scaleViewport.begin.x = _viewport.begin.x * scale_width;
        scaleViewport.begin.y = _viewport.begin.y * scale_height;
        scaleViewport.end.x = _viewport.end.x * scale_width;
        scaleViewport.end.y = _viewport.end.y * scale_height;

        scaleViewport.begin += renderViewport.begin;
        scaleViewport.end += renderViewport.begin;

        float vp_x = MENGINE_FLOORF( scaleViewport.begin.x + 0.5f );
        float vp_y = MENGINE_FLOORF( scaleViewport.begin.y + 0.5f );
        //float vp_x = renderViewport.begin.x;
        //float vp_y = renderViewport.begin.y;

        float width = scaleViewport.getWidth();
        float height = scaleViewport.getHeight();

        float vp_width = MENGINE_FLOORF( width + 0.5f );
        float vp_height = MENGINE_FLOORF( height + 0.5f );
        //float vp_width = width;
        //float vp_height = height;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );

        if( vp_x >= windowSize.x ||
            vp_y >= windowSize.y ||
            vp_x + vp_width <= 0.f ||
            vp_y + vp_height <= 0.f )
        {
            scaleViewport.begin.x = 0.f;
            scaleViewport.begin.y = 0.f;
            scaleViewport.end.x = 0.f;
            scaleViewport.end.y = 0.f;
        }
        else
        {
            if( vp_x < 0.f )
            {
                scaleViewport.begin.x = 0.f;
            }

            if( vp_x + vp_width > windowSize.x )
            {
                scaleViewport.end.x = windowSize.x;
            }

            if( vp_y < 0.f )
            {
                scaleViewport.begin.y = 0.f;
            }

            if( vp_y + vp_height > windowSize.y )
            {
                scaleViewport.end.y = windowSize.y;
            }
        }

        *_renderViewport = scaleViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::beginRenderPass( const RenderVertexBufferInterfacePtr & _vertexBuffer
        , const RenderIndexBufferInterfacePtr & _indexBuffer
        , const RenderProgramVariableInterfacePtr & _programVariable
        , const RenderContext * _context )
    {
        if( m_currentRenderVertexBuffer != _vertexBuffer )
        {
            m_currentRenderVertexBuffer = _vertexBuffer;

            m_renderSystem->setVertexBuffer( m_currentRenderVertexBuffer );
        }

        if( m_currentRenderIndexBuffer != _indexBuffer )
        {
            m_currentRenderIndexBuffer = _indexBuffer;

            m_renderSystem->setIndexBuffer( m_currentRenderIndexBuffer );
        }

        const RenderViewportInterface * viewport = _context->viewport;

        if( viewport != nullptr )
        {
            if( m_currentRenderContext.viewport != viewport )
            {
                const Viewport & v = viewport->getViewport();

                Viewport rv;
                this->calcRenderViewport_( v, &rv );

                m_renderSystem->setViewport( rv );

                m_currentRenderContext.viewport = viewport;
            }
        }
        else
        {
            if( m_currentRenderContext.viewport != nullptr )
            {
                uint32_t width = m_contentResolution.getWidth();
                uint32_t height = m_contentResolution.getHeight();

                Viewport rv;
                rv.begin.x = 0.f;
                rv.begin.y = 0.f;
                rv.end.x = (float)width;
                rv.end.y = (float)height;

                m_renderSystem->setViewport( rv );

                m_currentRenderContext.viewport = nullptr;
            }
        }

        const RenderCameraInterface * camera = _context->camera;

        if( camera != nullptr )
        {
            if( m_currentRenderContext.camera != camera )
            {
                const mt::mat4f & viewMatrix = camera->getCameraViewMatrix();

                m_renderSystem->setViewMatrix( viewMatrix );

                const mt::mat4f & projectionMatrix = camera->getCameraProjectionMatrix();

                m_renderSystem->setProjectionMatrix( projectionMatrix );

                m_currentRenderContext.camera = camera;
            }
        }
        else
        {
            if( m_currentRenderContext.camera != nullptr )
            {
                mt::mat4f viewMatrix;
                mt::ident_m4( viewMatrix );

                m_renderSystem->setViewMatrix( viewMatrix );

                mt::mat4f projectionMatrix;
                mt::ident_m4( projectionMatrix );

                m_renderSystem->setProjectionMatrix( projectionMatrix );

                m_currentRenderContext.camera = nullptr;
            }
        }

        const RenderTransformationInterface * transformation = _context->transformation;

        if( transformation != nullptr )
        {
            if( m_currentRenderContext.transformation != transformation )
            {
                const mt::mat4f & worldMatrix = transformation->getTransformationWorldMatrix();

                m_renderSystem->setWorldMatrix( worldMatrix );

                m_currentRenderContext.transformation = transformation;
            }
        }
        else
        {
            if( m_currentRenderContext.transformation != nullptr )
            {
                const mt::mat4f & worldMatrix = mt::mat4f::identity();

                m_renderSystem->setWorldMatrix( worldMatrix );

                m_currentRenderContext.transformation = nullptr;
            }
        }

        const RenderScissorInterface * scissor = _context->scissor;

        if( scissor != nullptr )
        {
            if( m_currentRenderContext.scissor != scissor )
            {
                const Viewport & v = scissor->getScissorViewport();

                m_renderSystem->setScissor( v );

                m_currentRenderContext.scissor = scissor;
            }
        }
        else
        {
            if( m_currentRenderContext.scissor != nullptr )
            {
                m_renderSystem->removeScissor();

                m_currentRenderContext.scissor = nullptr;
            }
        }

        if( m_currentRenderProgramVariable != _programVariable )
        {
            m_currentRenderProgramVariable = _programVariable;

            //ToDo
        }

        const RenderTargetInterface * target = _context->target;

        if( target != nullptr )
        {
            if( target->begin() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::endRenderPass( const RenderContext * _context )
    {
        const RenderTargetInterface * target = _context->target;

        if( target != nullptr )
        {
            target->end();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::renderPrimitives( const RenderPrimitive * _primitives, uint32_t _count )
    {
        for( uint32_t index = 0; index != _count; ++index )
        {
            const RenderPrimitive * renderPrimitive = _primitives + index;

            this->renderPrimitive_( renderPrimitive );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderBatchInterfacePtr & RenderService::requestRenderBatch( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount, uint32_t _indexCount, const DocumentPtr & _doc )
    {
        for( const RenderBatchPtr & batch : m_renderBatches )
        {
            if( batch->process( _vertexAttribute, _vertexCount, _indexCount ) == false )
            {
                continue;
            }

            return batch;
        }

        for( const RenderBatchPtr & batch : m_cacheRenderBatches )
        {
            if( batch->process( _vertexAttribute, _vertexCount, _indexCount ) == false )
            {
                continue;
            }

            m_renderBatches.emplace_back( batch );

            return batch;
        }

        RenderBatchPtr new_batch = m_factoryRenderBatch->createObject( _doc );

        new_batch->setVertexAttribute( _vertexAttribute );

        uint32_t elementSize = _vertexAttribute->getElementSize();

        RenderVertexBufferInterfacePtr vertexBuffer = m_renderSystem->createVertexBuffer( elementSize, BT_DYNAMIC, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexBuffer );

        vertexBuffer->resize( MENGINE_RENDER_VERTEX_MAX_BATCH / 2 );

        new_batch->setVertexBuffer( vertexBuffer );

        uint32_t indexSize = sizeof( RenderIndex );

        RenderIndexBufferInterfacePtr indexBuffer = m_renderSystem->createIndexBuffer( indexSize, BT_DYNAMIC, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( indexBuffer );

        indexBuffer->resize( MENGINE_RENDER_VERTEX_MAX_BATCH / 2 );

        new_batch->setIndexBuffer( indexBuffer );

        m_cacheRenderBatches.emplace_back( new_batch );
        m_renderBatches.emplace_back( new_batch );

        const RenderBatchPtr & batch = m_cacheRenderBatches.back();

        if( batch->process( _vertexAttribute, _vertexCount, _indexCount ) == false )
        {
            return RenderBatchInterfacePtr::none();
        }

        return batch;
    }
    //////////////////////////////////////////////////////////////////////////
    VectorRenderVertex2D & RenderService::getDebugRenderVertex2D( uint32_t _count )
    {
        m_debugRenderVertices.emplace_back( VectorRenderVertex2D() );
        VectorRenderVertex2D & vertices_array = m_debugRenderVertices.back();
        vertices_array.resize( _count );

        return vertices_array;
    }
    //////////////////////////////////////////////////////////////////////////
    VectorRenderIndex & RenderService::getDebugRenderIndex( uint32_t _count )
    {
        m_debugRenderIndices.emplace_back( VectorRenderIndex() );
        VectorRenderIndex & indices_array = m_debugRenderIndices.back();
        indices_array.resize( _count );

        return indices_array;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::makeBatches_( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        for( const RenderBatchPtr & batch : m_cacheRenderBatches )
        {
            if( batch->lock() == false )
            {
                LOGGER_ERROR( "failed to lock batch" );

                return false;
            }
        }

        _renderPipeline->batch();

        for( const RenderBatchPtr & batch : m_cacheRenderBatches )
        {
            if( batch->unlock() == false )
            {
                LOGGER_ERROR( "failed to lock batch" );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::flushRender_( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        STATISTIC_RESET_INTEGER( "RenderBatch" );
        STATISTIC_RESET_INTEGER( "DrawIndexPrimitives" );

        if( this->makeBatches_( _renderPipeline ) == false )
        {
            return;
        }

        _renderPipeline->render();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::setVSync( bool _vSync )
    {
        if( m_vsync == _vSync )
        {
            return;
        }

        m_vsync = _vSync;

        if( m_windowCreated == false )
        {
            return;
        }

        m_renderSystem->setVSync( m_vsync );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::getVSync() const
    {
        return m_vsync;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::setRenderViewport( const Viewport & _renderViewport )
    {
        m_renderViewport = _renderViewport;

        LOGGER_INFO( "render", "set render viewport [%f %f %f %f]"
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.end.x
            , m_renderViewport.end.y
        );
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderService::getRenderViewport() const
    {
        MENGINE_ASSERTION_FATAL( m_renderViewport.end.x != 0.f && m_renderViewport.end.y != 0.f );

        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
}
