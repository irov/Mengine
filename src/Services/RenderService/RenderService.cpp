#include "RenderService.h"

#include "Interface/CodecInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/WatchdogInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "BatchRenderPipeline.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/RenderUtils.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"

#include "math/convex8.h"

#include "stdex/memorycopy.h"
#include "stdex/intrusive_ptr_base.h"

#include <math.h>
#include <memory.h>

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
        , m_currentRenderTextureStage( 0 )
        , m_maxVertexCount( 0 )
        , m_maxIndexCount( 0 )
        , m_depthBufferTestEnable( false )
        , m_depthBufferWriteEnable( false )
        , m_alphaBlendEnable( false )
        , m_currentRenderMaterialStage( nullptr )
        , m_currentMaterialId( 0 )
        , m_currentBlendSrc( BF_ONE )
        , m_currentBlendDst( BF_ONE )
        , m_currentBlendOp( BOP_ADD )
    {
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

        m_debugInfo.dips = 0;
        m_debugInfo.frameCount = 0;
        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;

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
            ->removePrototype( STRINGIZE_STRING_LOCAL( "RenderPipeline" ), STRINGIZE_STRING_LOCAL( "Batch" ) );

        m_renderBatches.clear();
        m_cacheRenderBatches.clear();

        m_debugRenderVertices.clear();
        m_debugRenderIndices.clear();

        m_nullTexture = nullptr;
        m_whitePixelTexture = nullptr;

        m_currentRenderVertexBuffer = nullptr;
        m_currentRenderIndexBuffer = nullptr;
        m_currentRenderProgramVariable = nullptr;
        m_defaultRenderProgramVariable = nullptr;
        m_currentRenderProgram = nullptr;

        m_currentRenderViewport = nullptr;
        m_currentRenderCamera = nullptr;
        m_currentRenderTransformation = nullptr;
        m_currentRenderScissor = nullptr;

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
    bool RenderService::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen, int32_t _FSAAType, int32_t _FSAAQuality )
    {
        m_windowResolution = _resolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;

        m_fullscreen = _fullscreen;

        LOGGER_INFO( "window resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
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

        bool RenderEnableAutoDepthStencil = CONFIG_VALUE( "Engine", "RenderEnableAutoDepthStencil", false );
        uint32_t RenderMultiSampleCount = CONFIG_VALUE( "Engine", "RenderMultiSampleCount", 2U );

        if( m_renderSystem->createRenderWindow( m_windowResolution, _bits, m_fullscreen, RenderEnableAutoDepthStencil, m_vsync, _FSAAType, _FSAAQuality, RenderMultiSampleCount ) == false )
        {
            return false;
        }

        m_windowCreated = true;

        this->restoreRenderSystemStates_();

        if( this->createNullTexture_() == false )
        {
            LOGGER_ERROR( "invalid create __null__ texture"
            );

            return false;
        }

        if( this->createWhitePixelTexture_() == false )
        {
            LOGGER_ERROR( "invalid create WhitePixel texture"
            );

            return false;
        }

        m_defaultRenderTextureStage.mipmap = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMipmap();

        m_defaultRenderTextureStage.minification = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMinification();

        m_defaultRenderTextureStage.magnification = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMagnification();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::destroyRenderWindow()
    {
        this->restoreRenderSystemStates_();

        m_nullTexture = nullptr;
        m_whitePixelTexture = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::createNullTexture_()
    {
        LOGGER_INFO( "create null texture" );

        uint32_t null_mipmaps = 1;
        uint32_t null_width = 2;
        uint32_t null_height = 2;
        uint32_t null_channels = 3;
        uint32_t null_depth = 1;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( null_mipmaps, null_width, null_height, null_channels, null_depth, PF_UNKNOWN, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, false, "invalid create null texture %d:%d"
            , null_width
            , null_height
        );

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = null_width;
        rect.bottom = null_height;

        size_t pitch = 0;
        void * textureData = image->lock( &pitch, 0, rect, false );

        MENGINE_ASSERTION_MEMORY_PANIC( textureData, false, "invalid lock null texture %d:%d"
            , null_width
            , null_height
        );

        uint8_t * buffer_textureData = static_cast<uint8_t *>(textureData);

        buffer_textureData[0] = 0xFF;
        buffer_textureData[1] = 0x00;
        buffer_textureData[2] = 0x00;

        buffer_textureData[3] = 0x00;
        buffer_textureData[4] = 0xFF;
        buffer_textureData[5] = 0x00;

        buffer_textureData += pitch;

        buffer_textureData[0] = 0x00;
        buffer_textureData[1] = 0x00;
        buffer_textureData[2] = 0xFF;

        buffer_textureData[3] = 0x00;
        buffer_textureData[4] = 0x00;
        buffer_textureData[5] = 0x00;

        image->unlock( 0, true );

        const FileGroupInterfacePtr & defaulFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        RENDERTEXTURE_SERVICE()
            ->cacheFileTexture( defaulFileGroup, STRINGIZE_FILEPATH_LOCAL( "__null__" ), texture );

        m_nullTexture = texture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::createWhitePixelTexture_()
    {
        LOGGER_INFO( "create white pixel texture" );

        uint32_t null_mipmaps = 1;
        uint32_t null_width = 2;
        uint32_t null_height = 2;
        uint32_t null_channels = 3;
        uint32_t null_depth = 1;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( null_mipmaps, null_width, null_height, null_channels, null_depth, PF_UNKNOWN, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, false, "invalid create null texture %d:%d"
            , null_width
            , null_height
        );

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = null_width;
        rect.bottom = null_height;

        size_t pitch = 0;
        void * textureData = image->lock( &pitch, 0, rect, false );

        MENGINE_ASSERTION_MEMORY_PANIC( textureData, false, "invalid lock null texture %d:%d"
            , null_width
            , null_height
        );

        uint8_t * buffer_textureData = static_cast<uint8_t *>(textureData);

        buffer_textureData[0] = 0xFF;
        buffer_textureData[1] = 0xFF;
        buffer_textureData[2] = 0xFF;

        buffer_textureData[3] = 0xFF;
        buffer_textureData[4] = 0xFF;
        buffer_textureData[5] = 0xFF;

        buffer_textureData += pitch;

        buffer_textureData[0] = 0xFF;
        buffer_textureData[1] = 0xFF;
        buffer_textureData[2] = 0xFF;

        buffer_textureData[3] = 0xFF;
        buffer_textureData[4] = 0xFF;
        buffer_textureData[5] = 0xFF;

        image->unlock( 0, true );

        const FileGroupInterfacePtr & defaulFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        RENDERTEXTURE_SERVICE()
            ->cacheFileTexture( defaulFileGroup, STRINGIZE_FILEPATH_LOCAL( "WhitePixel" ), texture );

        m_whitePixelTexture = texture;

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
    void RenderService::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;

        m_fullscreen = _fullscreen;

        LOGGER_INFO( "window resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
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

        //this->restoreRenderSystemStates_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::onWindowClose()
    {
        if( m_windowCreated == true )
        {
            m_renderSystem->onWindowClose();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::onDeviceLostPrepare()
    {
        if( m_windowCreated == false )
        {
            return;
        }

        this->restoreRenderSystemStates_();

        m_nullTexture = nullptr;
        m_whitePixelTexture = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::onDeviceLostRestore()
    {
        if( m_windowCreated == false )
        {
            return;
        }

        this->restoreRenderSystemStates_();

        this->createNullTexture_();
        this->createWhitePixelTexture_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::beginScene( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        this->restoreRenderSystemStates_();

        _renderPipeline->prepare();

        if( m_renderSystem->beginScene() == false )
        {
            return false;
        }

        bool RenderFrameClear = CONFIG_VALUE( "Engine", "RenderFrameClear", true );

        if( RenderFrameClear == true )
        {            
            uint8_t RenderFrameClearColorR = CONFIG_VALUET( "Engine", "RenderFrameClearColorR", 0U, uint8_t );
            uint8_t RenderFrameClearColorG = CONFIG_VALUET( "Engine", "RenderFrameClearColorG", 0U, uint8_t );
            uint8_t RenderFrameClearColorB = CONFIG_VALUET( "Engine", "RenderFrameClearColorB", 0U, uint8_t );

            Color argb = Helper::makeColor( RenderFrameClearColorR, RenderFrameClearColorG, RenderFrameClearColorB, 255 );

            uint32_t frameBufferTypes = FBT_COLOR;

            bool RenderEnableAutoDepthStencil = CONFIG_VALUE( "Engine", "RenderEnableAutoDepthStencil", false );

            if( RenderEnableAutoDepthStencil == true )
            {
                frameBufferTypes |= FBT_DEPTH;
            }

            m_renderSystem->clearFrameBuffer( frameBufferTypes, argb, 1.f, 0x00000000 );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::endScene( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        _renderPipeline->flush();

        this->flushRender_( _renderPipeline );

        m_renderSystem->endScene();

        m_debugInfo.frameCount += 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::swapBuffers()
    {
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
            const RenderTextureStage & texture_stage = m_currentRenderMaterialStage->textureStage[stageId];

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

        if( m_alphaBlendEnable != m_currentRenderMaterialStage->alphaBlendEnable )
        {
            m_alphaBlendEnable = m_currentRenderMaterialStage->alphaBlendEnable;

            m_renderSystem->setAlphaBlendEnable( m_alphaBlendEnable );
        }

        if( m_depthBufferTestEnable != m_currentRenderMaterialStage->depthBufferTestEnable )
        {
            m_depthBufferTestEnable = m_currentRenderMaterialStage->depthBufferTestEnable;

            m_renderSystem->setDepthBufferTestEnable( m_depthBufferTestEnable );
        }

        if( m_depthBufferWriteEnable != m_currentRenderMaterialStage->depthBufferWriteEnable )
        {
            m_depthBufferWriteEnable = m_currentRenderMaterialStage->depthBufferWriteEnable;

            m_renderSystem->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
        }

        if( m_currentBlendSrc != m_currentRenderMaterialStage->blendSrc ||
            m_currentBlendDst != m_currentRenderMaterialStage->blendDst ||
            m_currentBlendOp != m_currentRenderMaterialStage->blendOp )
        {
            m_currentBlendSrc = m_currentRenderMaterialStage->blendSrc;
            m_currentBlendDst = m_currentRenderMaterialStage->blendDst;
            m_currentBlendOp = m_currentRenderMaterialStage->blendOp;

            m_renderSystem->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp );
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

            m_renderSystem->setTexture( _stageId, image );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::updateMaterial_( RenderMaterialInterface * _material )
    {
        uint32_t materialId = _material->getId();

        if( m_currentMaterialId == materialId )
        {
            return;
        }

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

        const RenderMaterialStage * stage = _material->getStage();

        this->updateStage_( stage );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::renderPrimitive_( RenderPrimitive * _renderPrimitive )
    {
        if( _renderPrimitive->indexCount == 0 )
        {
            IntrusivePtrBase::intrusive_ptr_release( _renderPrimitive->material );
            _renderPrimitive->material = nullptr;

            return;
        }

        RenderMaterialInterface * material = _renderPrimitive->material;

        this->updateMaterial_( material );

        m_renderSystem->updateProgram( m_currentRenderProgram );

        if( m_currentRenderProgramVariable != nullptr )
        {
            m_renderSystem->setProgramVariable( m_currentRenderProgramVariable, m_currentRenderProgram );
        }
        else
        {
            m_renderSystem->setProgramVariable( nullptr, m_currentRenderProgram );
        }

        EPrimitiveType primitiveType = material->getPrimitiveType();

        m_renderSystem->drawIndexedPrimitive(
            primitiveType,
            _renderPrimitive->baseVertexIndex,
            _renderPrimitive->minIndex,
            _renderPrimitive->vertexCount,
            _renderPrimitive->startIndex,
            _renderPrimitive->indexCount
        );

        IntrusivePtrBase::intrusive_ptr_release( _renderPrimitive->material );
        _renderPrimitive->material = nullptr;

        ++m_debugInfo.dips;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::restoreTextureStage_( uint32_t _stage )
    {
        m_renderTextureStages[_stage] = m_defaultRenderTextureStage;

        m_currentTexturesID[_stage] = 0;

        m_renderSystem->setTexture( _stage, nullptr );

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

        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::restoreRenderSystemStates_()
    {
        this->restoreRenderFrameStates_();

        m_currentRenderCamera = nullptr;

        mt::mat4f viewMatrix;
        mt::ident_m4( viewMatrix );

        m_renderSystem->setViewMatrix( viewMatrix );

        mt::mat4f projectionMatrix;
        mt::ident_m4( projectionMatrix );

        m_renderSystem->setProjectionMatrix( projectionMatrix );

        m_currentRenderViewport = nullptr;

        uint32_t width = m_windowResolution.getWidth();
        uint32_t height = m_windowResolution.getHeight();

        Viewport renderViewport;
        renderViewport.begin.x = 0.f;
        renderViewport.begin.y = 0.f;
        renderViewport.end.x = (float)width;
        renderViewport.end.y = (float)height;

        m_renderSystem->setViewport( renderViewport );

        m_currentRenderTransformation = nullptr;

        const mt::mat4f & worldMatrix = mt::mat4f::identity();

        m_renderSystem->setWorldMatrix( worldMatrix );

        m_currentRenderScissor = nullptr;

        m_renderSystem->removeScissor();

        m_currentMaterialId = 0;
        m_currentRenderTextureStage = 0;
        m_currentRenderMaterialStage = nullptr;

        m_currentBlendSrc = BF_ONE;
        m_currentBlendDst = BF_ZERO;
        m_currentBlendOp = BOP_ADD;

        m_depthBufferTestEnable = false;
        m_depthBufferWriteEnable = false;
        m_alphaBlendEnable = false;

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            this->restoreTextureStage_( i );
        }

        std::fill_n( m_currentTexturesID, MENGINE_MAX_TEXTURE_STAGES, 0 );

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
        m_renderSystem->setDepthBufferTestEnable( m_depthBufferTestEnable );
        m_renderSystem->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
        m_renderSystem->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
        m_renderSystem->setAlphaBlendEnable( m_alphaBlendEnable );
        m_renderSystem->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::calcRenderViewport_( const Viewport & _viewport, Viewport & _renderViewport ) const
    {
        float renderWidth = m_renderViewport.getWidth();
        float renderHeight = m_renderViewport.getHeight();

        uint32_t contentWidth = m_contentResolution.getWidth();
        uint32_t contentHeight = m_contentResolution.getHeight();

        float scale_width = renderWidth / float( contentWidth );
        float scale_height = renderHeight / float( contentHeight );

        Viewport renderViewport;
        renderViewport.begin.x = _viewport.begin.x * scale_width;
        renderViewport.begin.y = _viewport.begin.y * scale_height;
        renderViewport.end.x = _viewport.end.x * scale_width;
        renderViewport.end.y = _viewport.end.y * scale_height;

        renderViewport.begin += m_renderViewport.begin;
        renderViewport.end += m_renderViewport.begin;

        float vp_x = ::floorf( renderViewport.begin.x + 0.5f );
        float vp_y = ::floorf( renderViewport.begin.y + 0.5f );
        //float vp_x = renderViewport.begin.x;
        //float vp_y = renderViewport.begin.y;

        float width = renderViewport.getWidth();
        float height = renderViewport.getHeight();

        float vp_width = ::floorf( width + 0.5f );
        float vp_height = ::floorf( height + 0.5f );
        //float vp_width = width;
        //float vp_height = height;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( windowSize );

        if( vp_x >= windowSize.x ||
            vp_y >= windowSize.y ||
            vp_x + vp_width <= 0.f ||
            vp_y + vp_height <= 0.f )
        {
            renderViewport.begin.x = 0.f;
            renderViewport.begin.y = 0.f;
            renderViewport.end.x = 0.f;
            renderViewport.end.y = 0.f;
        }
        else
        {
            if( vp_x < 0.f )
            {
                renderViewport.begin.x = 0.f;
            }

            if( vp_x + vp_width > windowSize.x )
            {
                renderViewport.end.x = windowSize.x;
            }

            if( vp_y < 0.f )
            {
                renderViewport.begin.y = 0.f;
            }

            if( vp_y + vp_height > windowSize.y )
            {
                renderViewport.end.y = windowSize.y;
            }
        }

        _renderViewport = renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderServiceDebugInfo & RenderService::getDebugInfo() const
    {
        return m_debugInfo;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::resetFrameCount()
    {
        m_debugInfo.frameCount = 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::beginRenderPass( const RenderVertexBufferInterfacePtr & _vertexBuffer,
        const RenderIndexBufferInterfacePtr & _indexBuffer,
        const RenderViewportInterfacePtr & _viewport,
        const RenderCameraInterfacePtr & _camera,
        const RenderTransformationInterfacePtr & _transformation,
        const RenderScissorInterfacePtr & _scissor,
        const RenderTargetInterfacePtr & _target,
        const RenderProgramVariableInterfacePtr & _programVariable )
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

        if( _viewport != nullptr )
        {
            if( m_currentRenderViewport != _viewport )
            {
                const Viewport & viewport = _viewport->getViewport();

                Viewport renderViewport;
                this->calcRenderViewport_( viewport, renderViewport );

                m_renderSystem->setViewport( renderViewport );

                m_currentRenderViewport = _viewport;
            }
        }
        else
        {
            if( m_currentRenderViewport != nullptr )
            {
                uint32_t width = m_contentResolution.getWidth();
                uint32_t height = m_contentResolution.getHeight();

                Viewport renderViewport;
                renderViewport.begin.x = 0.f;
                renderViewport.begin.y = 0.f;
                renderViewport.end.x = (float)width;
                renderViewport.end.y = (float)height;

                m_renderSystem->setViewport( renderViewport );

                m_currentRenderViewport = nullptr;
            }
        }

        if( _camera != nullptr )
        {
            if( m_currentRenderCamera != _camera )
            {
                const mt::mat4f & viewMatrix = _camera->getCameraViewMatrix();

                m_renderSystem->setViewMatrix( viewMatrix );

                const mt::mat4f & projectionMatrix = _camera->getCameraProjectionMatrix();

                m_renderSystem->setProjectionMatrix( projectionMatrix );

                m_currentRenderCamera = _camera;
            }
        }
        else
        {
            if( m_currentRenderCamera != nullptr )
            {
                mt::mat4f viewMatrix;
                mt::ident_m4( viewMatrix );

                m_renderSystem->setViewMatrix( viewMatrix );

                mt::mat4f projectionMatrix;
                mt::ident_m4( projectionMatrix );

                m_renderSystem->setProjectionMatrix( projectionMatrix );

                m_currentRenderCamera = nullptr;
            }
        }

        if( _transformation != nullptr )
        {
            if( m_currentRenderTransformation != _transformation )
            {
                const mt::mat4f & worldMatrix = _transformation->getTransformationWorldMatrix();

                m_renderSystem->setWorldMatrix( worldMatrix );

                m_currentRenderTransformation = _transformation;
            }
        }
        else
        {
            if( m_currentRenderTransformation != nullptr )
            {
                const mt::mat4f & worldMatrix = mt::mat4f::identity();

                m_renderSystem->setWorldMatrix( worldMatrix );

                m_currentRenderTransformation = nullptr;
            }
        }

        if( _scissor != nullptr )
        {
            if( m_currentRenderScissor != _scissor )
            {
                const Viewport & viewport = _scissor->getScissorViewport();

                m_renderSystem->setScissor( viewport );

                m_currentRenderScissor = _scissor;
            }
        }
        else
        {
            if( m_currentRenderScissor != nullptr )
            {
                m_renderSystem->removeScissor();

                m_currentRenderScissor = nullptr;
            }
        }

        if( _target != nullptr )
        {
            if( _target->begin() == false )
            {
                return false;
            }
        }

        if( m_currentRenderProgramVariable != _programVariable )
        {
            m_currentRenderProgramVariable = _programVariable;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::endRenderPass( const RenderTargetInterfacePtr & _target )
    {
        if( _target != nullptr )
        {
            _target->end();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::renderPrimitives( RenderPrimitive * _primitives, uint32_t _count )
    {
        for( uint32_t index = 0 ; index != _count; ++index )
        {
            RenderPrimitive * renderPrimitive = _primitives + index;

            this->renderPrimitive_( renderPrimitive );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderBatchInterfacePtr & RenderService::requestRenderBatch( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount, uint32_t _indexCount )
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

        RenderBatchPtr new_batch = m_factoryRenderBatch->createObject( MENGINE_DOCUMENT_FACTORABLE );

        new_batch->setVertexAttribute( _vertexAttribute );

        uint32_t elementSize = _vertexAttribute->getElementSize();

        RenderVertexBufferInterfacePtr vertexBuffer = m_renderSystem->createVertexBuffer( elementSize, BT_DYNAMIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexBuffer, RenderBatchInterfacePtr::none() );

        vertexBuffer->resize( MENGINE_RENDER_VERTEX_MAX_BATCH / 2 );

        new_batch->setVertexBuffer( vertexBuffer );

        uint32_t indexSize = sizeof( RenderIndex );

        RenderIndexBufferInterfacePtr indexBuffer = m_renderSystem->createIndexBuffer( indexSize, BT_DYNAMIC, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( indexBuffer, RenderBatchInterfacePtr::none() );

        indexBuffer->resize( MENGINE_RENDER_VERTEX_MAX_BATCH / 2 );

        new_batch->setIndexBuffer( indexBuffer );

        m_cacheRenderBatches.emplace_back( new_batch );

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
        m_debugInfo.batch = 0;
        m_debugInfo.dips = 0;

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

        if( m_windowCreated == true )
        {
            m_renderSystem->setVSync( m_vsync );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderService::getVSync() const
    {
        return m_vsync;
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderService::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::calcQuadSquare_( const RenderVertex2D * _vertex, uint32_t _vertexCount, const Viewport & _viewport )
    {
        uint32_t triangleCount2 = (_vertexCount / 4);

        for( uint32_t i = 0; i != triangleCount2; ++i )
        {
            const RenderVertex2D & v0 = _vertex[i * 4 + 0];
            const RenderVertex2D & v1 = _vertex[i * 4 + 1];
            const RenderVertex2D & v2 = _vertex[i * 4 + 2];
            const RenderVertex2D & v3 = _vertex[i * 4 + 3];

            mt::convex8 tri1;
            tri1.add( v0.position.to_vec2f() );
            tri1.add( v1.position.to_vec2f() );
            tri1.add( v2.position.to_vec2f() );

            mt::convex8 tri2;
            tri2.add( v0.position.to_vec2f() );
            tri2.add( v2.position.to_vec2f() );
            tri2.add( v3.position.to_vec2f() );

            mt::convex8 cv;
            cv.add( mt::vec2f( _viewport.begin.x, _viewport.begin.y ) );
            cv.add( mt::vec2f( _viewport.end.x, _viewport.begin.y ) );
            cv.add( mt::vec2f( _viewport.end.x, _viewport.end.y ) );
            cv.add( mt::vec2f( _viewport.begin.x, _viewport.end.y ) );

            mt::convex8 tric1;
            mt::convex8_intersect( tri1, cv, tric1 );

            mt::convex8 tric2;
            mt::convex8_intersect( tri2, cv, tric2 );

            float tric1_area = mt::convex8_area( tric1 );
            float tric2_area = mt::convex8_area( tric2 );

            m_debugInfo.fillrate += tric1_area;
            m_debugInfo.fillrate += tric2_area;
        }

        m_debugInfo.triangle += triangleCount2 * 2;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderService::calcMeshSquare_( const RenderVertex2D * _vertex, uint32_t _vertexCount, const RenderIndex * _indices, uint32_t _indicesCount, const Viewport & _viewport )
    {
        MENGINE_UNUSED( _vertexCount );

        for( uint32_t i = 0; i != (_indicesCount / 3); ++i )
        {
            RenderIndex i0 = _indices[i + 0];
            RenderIndex i1 = _indices[i + 1];
            RenderIndex i2 = _indices[i + 2];

            const RenderVertex2D & v0 = _vertex[i0];
            const RenderVertex2D & v1 = _vertex[i1];
            const RenderVertex2D & v2 = _vertex[i2];

            mt::convex8 tri;
            tri.add( v0.position.to_vec2f() );
            tri.add( v1.position.to_vec2f() );
            tri.add( v2.position.to_vec2f() );

            mt::convex8 cv;
            cv.add( mt::vec2f( _viewport.begin.x, _viewport.begin.y ) );
            cv.add( mt::vec2f( _viewport.end.x, _viewport.begin.y ) );
            cv.add( mt::vec2f( _viewport.end.x, _viewport.end.y ) );
            cv.add( mt::vec2f( _viewport.begin.x, _viewport.end.y ) );

            mt::convex8 tric;
            mt::convex8_intersect( tri, cv, tric );

            float tric_area = mt::convex8_area( tric );

            m_debugInfo.fillrate += tric_area;

            m_debugInfo.triangle += 1;
        }
    }
}
