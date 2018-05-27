#include "RenderEngine.h"

#include "Interface/CodecInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/WatchdogInterface.h"
#include "Interface/ConfigInterface.h"

#include "Factory/FactoryPool.h"

#include "Core/RenderUtils.h"

#include "math/convex8.h"

#include "Logger/Logger.h"

#include "stdex/memorycopy.h"
#include "stdex/intrusive_ptr_base.h"

#include <math.h>
#include <memory.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderService, Mengine::RenderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    static const uint32_t RenderVertexBatchMax = 65536U;
    //////////////////////////////////////////////////////////////////////////
    //static bool hasRenderObjectFlag( const RenderObject * ro, uint32_t _flag )
    //{
    //    return (ro->flags & _flag) > 0;
    //}
    //////////////////////////////////////////////////////////////////////////
    RenderEngine::RenderEngine()
        : m_windowCreated( false )
        , m_vsync( false )
        , m_fullscreen( false )
        , m_currentTextureStages( 0 )
        , m_maxVertexCount( 0 )
        , m_maxIndexCount( 0 )
        , m_depthBufferWriteEnable( false )
        , m_alphaBlendEnable( false )
        , m_debugFillrateCalcMode( false )
        , m_debugStepRenderMode( false )
        , m_debugRedAlertMode( false )
        , m_currentStage( nullptr )
        , m_batchMode( ERBM_NORMAL )
        , m_currentMaterialId( 0 )
        , m_iterateRenderObjects( 0 )
        , m_limitRenderObjects( 0 )
        , m_stopRenderObjects( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderEngine::~RenderEngine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::_initializeService()
    {
        m_maxVertexCount = CONFIG_VALUE( "Engine", "RenderMaxVertexCount", 32000U );
        m_maxIndexCount = CONFIG_VALUE( "Engine", "RenderMaxIndexCount", 48000U );

        uint32_t maxObjects = CONFIG_VALUE( "Engine", "RenderMaxObject", 8000U );
        uint32_t maxPasses = CONFIG_VALUE( "Engine", "RenderMaxPass", 200U );

        uint32_t maxQuadBatch = CONFIG_VALUE( "Engine", "RenderMaxQuadBatch", 2000U );
        uint32_t maxLineBatch = CONFIG_VALUE( "Engine", "RenderMaxLineBatch", 4000U );

        m_renderObjects.reserve( maxObjects );
        m_renderPasses.reserve( maxPasses );

        m_indicesQuad.resize( maxQuadBatch * 6 );
        m_indicesLine.resize( maxLineBatch * 1 );

        for( uint32_t i = 0; i != maxQuadBatch; ++i )
        {
            uint32_t indexOffset = i * 6;

            RenderIndex vertexOffset = (RenderIndex)i * 4;

            m_indicesQuad[indexOffset + 0] = vertexOffset + 0;
            m_indicesQuad[indexOffset + 1] = vertexOffset + 3;
            m_indicesQuad[indexOffset + 2] = vertexOffset + 1;
            m_indicesQuad[indexOffset + 3] = vertexOffset + 1;
            m_indicesQuad[indexOffset + 4] = vertexOffset + 3;
            m_indicesQuad[indexOffset + 5] = vertexOffset + 2;
        }

        for( uint32_t i = 0; i != maxLineBatch; ++i )
        {
            RenderIndex vertexOffset = (RenderIndex)i;

            m_indicesLine[i] = vertexOffset;
        }

        m_debugInfo.dips = 0;
        m_debugInfo.frameCount = 0;
        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;
        
        //m_megatextures = new Megatextures(2048.f, 2048.f, PF_A8R8G8B8);

        uint32_t batchMode = CONFIG_VALUE( "Engine", "RenderServiceBatchMode", 1 );

        switch( batchMode )
        {
        case 0U:
            {
                m_batchMode = ERBM_NONE;
            }break;
        case 1U:
            {
                m_batchMode = ERBM_NORMAL;
            }break;
        case 2U:
            {
                m_batchMode = ERBM_SMART;
            }break;
        default:
            {
                m_batchMode = ERBM_SMART;
            }break;
        }

        m_factoryRenderBatch = new FactoryPool<RenderBatch, 16>();
        m_factoryRenderPass = new FactoryPool<RenderPass, 128>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::_finalizeService()
    {
        if( m_windowCreated == false )
        {
            return;
        }

        this->restoreRenderSystemStates_();

        for( TArrayRenderObject::iterator
            it = m_renderObjects.begin(),
            it_end = m_renderObjects.end();
            it != it_end;
            ++it )
        {
            RenderObject & ro = *it;

            stdex::intrusive_ptr_release( ro.material );
        }

        m_renderObjects.clear();
        m_renderBatches.clear();
        m_renderPasses.clear();
        //m_textures.clear();

        m_nullTexture = nullptr;
        m_whitePixelTexture = nullptr;

        m_currentRenderViewport = nullptr;
        m_currentRenderCamera = nullptr;
        m_currentRenderScissor = nullptr;

        m_currentVertexBuffer = nullptr;
        m_currentIndexBuffer = nullptr;

        m_currentProgram = nullptr;

        m_currentRenderTarget = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderBatch );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderPass );

        m_factoryRenderBatch = nullptr;
        m_factoryRenderPass = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen,
        int _FSAAType, int _FSAAQuality )
    {
        m_windowResolution = _resolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;

        m_fullscreen = _fullscreen;

        LOGGER_INFO( "RenderEngine::createRenderWindow:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
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

        uint32_t MultiSampleCount = CONFIG_VALUE( "Engine", "MultiSampleCount", 2U );

        m_windowCreated = RENDER_SYSTEM()
            ->createRenderWindow( m_windowResolution, _bits, m_fullscreen, m_vsync, _FSAAType, _FSAAQuality, MultiSampleCount );

        if( m_windowCreated == false )
        {
            return false;
        }

        this->restoreRenderSystemStates_();

        if( this->createNullTexture_() == false )
        {
            LOGGER_ERROR( "RenderEngine::createRenderWindow invalid create __null__ texture"
            );

            return false;
        }

        if( this->createWhitePixelTexture_() == false )
        {
            LOGGER_ERROR( "RenderEngine::createRenderWindow invalid create WhitePixel texture"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::destroyRenderWindow()
    {
        this->restoreRenderSystemStates_();

        m_nullTexture = nullptr;
        m_whitePixelTexture = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::createNullTexture_()
    {
        uint32_t null_mipmaps = 1;
        uint32_t null_width = 2;
        uint32_t null_height = 2;
        uint32_t null_channels = 3;
        uint32_t null_depth = 1;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( null_mipmaps, null_width, null_height, null_channels, null_depth, PF_UNKNOWN );

        if( texture == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::createNullTexture_ invalid create null texture %d:%d"
                , null_width
                , null_height
            );

            return false;
        }

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = null_width;
        rect.bottom = null_height;

        size_t pitch = 0;
        void * textureData = image->lock( &pitch, 0, rect, false );

        if( textureData == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::createNullTexture_ invalid lock null texture %d:%d"
                , null_width
                , null_height
            );

            return false;
        }

        unsigned char * buffer_textureData = static_cast<unsigned char *>(textureData);

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

        RENDERTEXTURE_SERVICE()
            ->cacheFileTexture( ConstString::none(), STRINGIZE_FILEPATH_LOCAL( "__null__" ), texture );

        m_nullTexture = texture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::createWhitePixelTexture_()
    {
        uint32_t null_mipmaps = 1;
        uint32_t null_width = 2;
        uint32_t null_height = 2;
        uint32_t null_channels = 3;
        uint32_t null_depth = 1;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( null_mipmaps, null_width, null_height, null_channels, null_depth, PF_UNKNOWN );

        if( texture == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::createWhitePixelTexture_ invalid create null texture %d:%d"
                , null_width
                , null_height
            );

            return false;
        }

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = null_width;
        rect.bottom = null_height;

        size_t pitch = 0;
        void * textureData = image->lock( &pitch, 0, rect, false );

        if( textureData == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::createWhitePixelTexture_ invalid lock null texture %d:%d"
                , null_width
                , null_height
            );

            return false;
        }

        unsigned char * buffer_textureData = static_cast<unsigned char *>(textureData);

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

        RENDERTEXTURE_SERVICE()
            ->cacheFileTexture( ConstString::none(), STRINGIZE_FILEPATH_LOCAL( "WhitePixel" ), texture );

        m_whitePixelTexture = texture;

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
    void RenderEngine::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;

        m_fullscreen = _fullscreen;

        LOGGER_INFO( "RenderEngine::changeWindowMode:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
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

        RENDER_SYSTEM()
            ->changeWindowMode( m_windowResolution, m_fullscreen );

        //this->restoreRenderSystemStates_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::screenshot( const RenderTextureInterfacePtr & _texture, const mt::vec4f & _rect )
    {
        const RenderImageInterfacePtr & image = _texture->getImage();

        RENDER_SYSTEM()
            ->screenshot( image, _rect );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::onWindowClose()
    {
        if( m_windowCreated == true )
        {
            RENDER_SYSTEM()
                ->onWindowClose();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::onDeviceLostPrepare()
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
    void RenderEngine::onDeviceLostRestore()
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
    bool RenderEngine::beginScene()
    {
        this->restoreRenderSystemStates_();

        if( RENDER_SYSTEM()->beginScene() == false )
        {
            return false;
        }

        RENDER_SYSTEM()
            ->clear( 0x00000000, true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::endScene()
    {
        this->flushRender_();

        RENDER_SYSTEM()
            ->endScene();

        m_debugInfo.frameCount += 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::swapBuffers()
    {
        RENDER_SYSTEM()
            ->swapBuffers();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::isWindowCreated() const
    {
        return m_windowCreated;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::updateStage_( const RenderMaterialStage * _stage )
    {
        if( m_currentStage == _stage )
        {
            return;
        }

        m_currentStage = _stage;

        for( uint32_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
        {
            RenderTextureStage & current_texture_stage = m_currentTextureStage[stageId];
            const RenderTextureStage & texture_stage = m_currentStage->textureStage[stageId];

            if( current_texture_stage.addressU != texture_stage.addressU
                || current_texture_stage.addressV != texture_stage.addressV
                || current_texture_stage.addressBorder != texture_stage.addressBorder )
            {
                current_texture_stage.addressU = texture_stage.addressU;
                current_texture_stage.addressV = texture_stage.addressV;
                current_texture_stage.addressBorder = texture_stage.addressBorder;

                RENDER_SYSTEM()->setTextureAddressing( stageId
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

                RENDER_SYSTEM()->setTextureStageFilter( stageId
                    , current_texture_stage.minification
                    , current_texture_stage.mipmap
                    , current_texture_stage.magnification
                );
            }
        }

        if( m_alphaBlendEnable != m_currentStage->alphaBlendEnable )
        {
            m_alphaBlendEnable = m_currentStage->alphaBlendEnable;

            RENDER_SYSTEM()
                ->setAlphaBlendEnable( m_alphaBlendEnable );
        }

        if( m_currentBlendSrc != m_currentStage->blendSrc ||
            m_currentBlendDst != m_currentStage->blendDst ||
            m_currentBlendOp != m_currentStage->blendOp )
        {
            m_currentBlendSrc = m_currentStage->blendSrc;
            m_currentBlendDst = m_currentStage->blendDst;
            m_currentBlendOp = m_currentStage->blendOp;

            RENDER_SYSTEM()
                ->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp );
        }

        if( m_currentProgram != m_currentStage->program )
        {
            m_currentProgram = m_currentStage->program;

            RENDER_SYSTEM()
                ->setProgram( m_currentProgram );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::updateTexture_( uint32_t _stageId, const RenderTextureInterfacePtr & _texture )
    {
        uint32_t texture_id = _texture->getId();
        uint32_t current_texture_id = m_currentTexturesID[_stageId];

        if( texture_id != current_texture_id || current_texture_id == 0 )
        {
            m_currentTexturesID[_stageId] = texture_id;

            const RenderImageInterfacePtr & image = _texture->getImage();

            RENDER_SYSTEM()
                ->setTexture( _stageId, image );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::updateMaterial_( RenderMaterialInterface * _material )
    {
        uint32_t materialId = _material->getId();

        if( m_currentMaterialId == materialId )
        {
            return;
        }

        m_currentMaterialId = materialId;

        uint32_t textureCount = _material->getTextureCount();

        if( m_currentTextureStages > textureCount )
        {
            for( uint32_t stageId = textureCount; stageId != m_currentTextureStages; ++stageId )
            {
                this->restoreTextureStage_( stageId );
            }
        }

        m_currentTextureStages = textureCount;

        for( uint32_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
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
    void RenderEngine::renderObject_( RenderObject * _renderObject )
    {
        if( _renderObject->dipIndiciesNum == 0 )
        {
            return;
        }

        RenderMaterialInterface * material = _renderObject->material;

        this->updateMaterial_( material );

        RENDER_SYSTEM()
            ->updateProgram( m_currentProgram );

        if( m_currentIndexBuffer != _renderObject->indexBuffer )
        {
            m_currentIndexBuffer = _renderObject->indexBuffer;

            RENDER_SYSTEM()
                ->setIndexBuffer( m_currentIndexBuffer );
        }

        if( m_currentVertexBuffer != _renderObject->vertexBuffer )
        {
            m_currentVertexBuffer = _renderObject->vertexBuffer;

            RENDER_SYSTEM()
                ->setVertexBuffer( m_currentVertexBuffer );
        }

        EPrimitiveType primitiveType = material->getPrimitiveType();

        RENDER_SYSTEM()->drawIndexedPrimitive(
            primitiveType,
            _renderObject->baseVertexIndex,
            _renderObject->minIndex,
            _renderObject->dipVerticesNum,
            _renderObject->startIndex,
            _renderObject->dipIndiciesNum
        );

        stdex::intrusive_ptr_release( _renderObject->material );
        stdex::intrusive_ptr_release( _renderObject->vertexBuffer );
        stdex::intrusive_ptr_release( _renderObject->indexBuffer );

        ++m_debugInfo.dips;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::restoreTextureStage_( uint32_t _stage )
    {
        RenderTextureStage & stage = m_currentTextureStage[_stage];

        stage = RenderTextureStage();
        
        stage.mipmap = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMipmap();

        stage.minification = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMinification();

        stage.magnification = RENDERMATERIAL_SERVICE()
            ->getDefaultTextureFilterMagnification();

        m_currentTexturesID[_stage] = 0;

        RENDER_SYSTEM()
            ->setTexture( _stage, nullptr );

        RENDER_SYSTEM()->setTextureAddressing( _stage
            , stage.addressU
            , stage.addressV
            , stage.addressBorder
        );

        RENDER_SYSTEM()->setTextureStageFilter( _stage
            , stage.minification
            , stage.mipmap
            , stage.magnification
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::restoreRenderSystemStates_()
    {
        m_renderBatches.clear();
        m_renderPasses.clear();
        m_renderObjects.clear();

        m_debugVertices.clear();

        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;

        m_iterateRenderObjects = 0;
        m_stopRenderObjects = false;

        m_currentRenderCamera = nullptr;
        m_currentRenderViewport = nullptr;
        m_currentRenderScissor = nullptr;
        m_currentRenderTarget = nullptr;

        m_currentMaterialId = 0;
        m_currentTextureStages = 0;
        m_currentStage = nullptr;

        m_currentBlendSrc = BF_ONE;
        m_currentBlendDst = BF_ZERO;
        m_currentBlendOp = BOP_ADD;

        m_depthBufferWriteEnable = false;
        m_alphaBlendEnable = false;

        for( int i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            this->restoreTextureStage_( i );
        }

        std::fill_n( m_currentTexturesID, MENGINE_MAX_TEXTURE_STAGES, 0 );

        m_currentVertexBuffer = nullptr;
        m_currentIndexBuffer = nullptr;
        m_currentProgram = nullptr;

        mt::mat4f viewTransform;
        mt::ident_m4( viewTransform );

        mt::mat4f projTransform;
        mt::ident_m4( projTransform );

        mt::mat4f worldTransform;
        mt::ident_m4( worldTransform );

        RENDER_SYSTEM()->setProjectionMatrix( projTransform );
        RENDER_SYSTEM()->setViewMatrix( viewTransform );
        RENDER_SYSTEM()->setWorldMatrix( worldTransform );
        RENDER_SYSTEM()->setVertexBuffer( m_currentVertexBuffer );
        RENDER_SYSTEM()->setIndexBuffer( m_currentIndexBuffer );
        RENDER_SYSTEM()->setProgram( m_currentProgram );
        RENDER_SYSTEM()->setCullMode( CM_CULL_NONE );
        //RENDER_SYSTEM()->setFillMode( FM_SOLID );
        //RENDER_SYSTEM()->setFillMode( FM_WIREFRAME );
        RENDER_SYSTEM()->setDepthBufferTestEnable( false );
        RENDER_SYSTEM()->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
        RENDER_SYSTEM()->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
        RENDER_SYSTEM()->setAlphaBlendEnable( m_alphaBlendEnable );
        RENDER_SYSTEM()->setBlendFactor( m_currentBlendSrc, m_currentBlendDst, m_currentBlendOp );

        LOGGER_INFO( "RenderEngine::restoreRenderSystemStates_ texture stages %d"
            , MENGINE_MAX_TEXTURE_STAGES
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport, float zn, float zf )
    {
        RENDER_SYSTEM()
            ->makeProjectionOrthogonal( _projectionMatrix, _viewport, zn, zf );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf )
    {
        RENDER_SYSTEM()
            ->makeProjectionPerspective( _projectionMatrix, _fov, _aspect, zn, zf );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf )
    {
        RENDER_SYSTEM()
            ->makeProjectionFrustum( _projectionMatrix, _viewport, zn, zf );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport )
    {
        RENDER_SYSTEM()
            ->makeViewMatrixFromViewport( _viewMatrix, _viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
    {
        RENDER_SYSTEM()
            ->makeViewMatrixLookAt( _viewMatrix, _eye, _dir, _up, _sign );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::calcRenderViewport_( const Viewport & _viewport, Viewport & _renderViewport ) const
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
    const RenderServiceDebugInfo & RenderEngine::getDebugInfo() const
    {
        return m_debugInfo;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::resetFrameCount()
    {
        m_debugInfo.frameCount = 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::renderPasses_()
    {
        if( m_renderPasses.empty() == true )	// nothing to render
        {
            return;
        }

        for( const RenderPassPtr & renderPass : m_renderPasses )
        {
            this->renderPass_( renderPass );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::renderPass_( const RenderPassPtr & _pass )
    {
        if( _pass->viewport != nullptr )
        {
            const Viewport & viewport = _pass->viewport->getViewport();

            Viewport renderViewport;
            this->calcRenderViewport_( viewport, renderViewport );

            RENDER_SYSTEM()
                ->setViewport( renderViewport );
        }
        else
        {
            uint32_t width = m_contentResolution.getWidth();
            uint32_t height = m_contentResolution.getHeight();

            Viewport renderViewport;
            renderViewport.begin.x = 0.f;
            renderViewport.begin.y = 0.f;
            renderViewport.end.x = (float)width;
            renderViewport.end.y = (float)height;

            RENDER_SYSTEM()
                ->setViewport( renderViewport );
        }

        if( _pass->camera != nullptr )
        {
            //const mt::mat4f & worldMatrix = _renderPass.camera->getCameraWorldMatrix();

            //RENDER_SYSTEM()
            //	->setWorldMatrix( worldMatrix );

            const mt::mat4f & viewMatrix = _pass->camera->getCameraViewMatrix();

            RENDER_SYSTEM()
                ->setViewMatrix( viewMatrix );

            const mt::mat4f & projectionMatrix = _pass->camera->getCameraProjectionMatrix();

            RENDER_SYSTEM()
                ->setProjectionMatrix( projectionMatrix );
        }
        else
        {
            //mt::mat4f worldMatrix;
            //mt::ident_m4( worldMatrix );

            //RENDER_SYSTEM()
            //	->setWorldMatrix( worldMatrix );

            mt::mat4f viewMatrix;
            mt::ident_m4( viewMatrix );

            RENDER_SYSTEM()
                ->setViewMatrix( viewMatrix );

            mt::mat4f projectionMatrix;
            mt::ident_m4( projectionMatrix );

            RENDER_SYSTEM()
                ->setProjectionMatrix( projectionMatrix );
        }

        if( _pass->scissor != nullptr )
        {
            const Viewport & viewport = _pass->scissor->getScissorViewport();

            RENDER_SYSTEM()
                ->setScissor( viewport );
        }
        else
        {
            RENDER_SYSTEM()
                ->removeScissor();
        }

        if( _pass->target != nullptr )
        {
            const RenderTargetInterfacePtr & target = _pass->target;

            if( target->begin() == false )
            {
                return;
            }
        }

        this->renderObjects_( _pass );

        if( _pass->target != nullptr )
        {
            const RenderTargetInterfacePtr & target = _pass->target;

            target->end();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::renderObjects_( const RenderPassPtr & _renderPass )
    {
        TArrayRenderObject::iterator it_begin = m_renderObjects.advance( _renderPass->beginRenderObject );
        TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass->beginRenderObject + _renderPass->countRenderObject );

        for( ; it_begin != it_end; ++it_begin )
        {
            RenderObject * renderObject = it_begin;

            this->renderObject_( renderObject );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderBatchPtr & RenderEngine::requestRenderBatch_( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount )
    {
        for( const RenderBatchPtr & batch : m_renderBatches )
        {
            if( batch->vertexAttribute != _vertexAttribute )
            {
                continue;
            }

            if( batch->vertexCount + _vertexCount >= RenderVertexBatchMax )
            {
                continue;
            }
                        
            return batch;
        }

        RenderBatchPtr new_batch = m_factoryRenderBatch->createObject();

        new_batch->vertexAttribute = _vertexAttribute;

        uint32_t elementSize = _vertexAttribute->getElementSize();

        new_batch->vertexBuffer = RENDER_SYSTEM()
            ->createVertexBuffer( elementSize, BT_STATIC );

        new_batch->vertexCount = 0;
        new_batch->vertexMemory = nullptr;

        new_batch->indexBuffer = RENDER_SYSTEM()
            ->createIndexBuffer( sizeof( RenderIndex ), BT_STATIC );

        new_batch->indexCount = 0;
        new_batch->indexMemory = nullptr;

        new_batch->vbPos = 0;
        new_batch->ibPos = 0;

        m_renderBatches.emplace_back( new_batch );

        const RenderBatchPtr & batch = m_renderBatches.back();

        return batch;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::testRenderPass_( const RenderState * _state, const RenderBatchPtr & _renderBatch ) const
    {
        if( m_renderPasses.empty() == true )
        {
            return true;
        }

        const RenderPassPtr & pass = m_renderPasses.back();

        if( pass->batch != _renderBatch ||
            pass->viewport != _state->viewport ||
            pass->camera != _state->camera ||
            pass->scissor != _state->scissor ||
            pass->target != _state->target )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderPassPtr & RenderEngine::requestRenderPass_( const RenderState * _state, const RenderMaterialInterfacePtr & _material, uint32_t _vertexCount, uint32_t _indexCount )
    {
        const RenderMaterialStage * materialStage = _material->getStage();

        const RenderProgramInterfacePtr & program = materialStage->program;

        const RenderVertexAttributeInterfacePtr & vertexAttribute = program->getVertexAttribute();

        const RenderBatchPtr & batch = this->requestRenderBatch_( vertexAttribute, _vertexCount );

        batch->vertexCount += _vertexCount;
        batch->indexCount += _indexCount;

        if( this->testRenderPass_( _state, batch ) == true )
        {
            RenderPassPtr pass = m_factoryRenderPass->createObject();

            pass->beginRenderObject = (uint32_t)m_renderObjects.size();
            pass->countRenderObject = 0U;
            pass->batch = batch;
            pass->viewport = _state->viewport;
            pass->camera = _state->camera;
            pass->scissor = _state->scissor;
            pass->target = _state->target;

            for( uint32_t i = 0U; i != MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX; ++i )
            {
                pass->materialEnd[i] = nullptr;
            }

            m_renderPasses.emplace_back( pass );
        }

        const RenderPassPtr & rp = m_renderPasses.back();

        return rp;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::addRenderMesh( const RenderState * _state, const RenderMaterialInterfacePtr & _material
        , const RenderVertexBufferInterfacePtr & _vertexBuffer
        , const RenderIndexBufferInterfacePtr & _indexBuffer
        , uint32_t _indexCount )
    {
        (void)_state;
        (void)_material;
        (void)_vertexBuffer;
        (void)_indexBuffer;
        (void)_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::addRenderObject( const RenderState * _state, const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const RenderIndex * _indices, uint32_t _indexCount
        , const mt::box2f * _bb, bool _debug )
    {
#ifndef NDEBUG
        if( _state == nullptr )
        {
            LOGGER_ERROR( "RenderEngine::renderObject2D _state == NULL"
            );

            return;
        }

        if( _state->viewport == nullptr )
        {
            LOGGER_ERROR( "RenderEngine::renderObject2D viewport == NULL"
            );

            return;
        }

        if( _state->camera == nullptr )
        {
            LOGGER_ERROR( "RenderEngine::renderObject2D camera == NULL"
            );

            return;
        }

        if( _material == nullptr )
        {
            LOGGER_ERROR( "RenderEngine::renderObject2D _material == NULL"
            );

            return;
        }

        if( _vertices == nullptr )
        {
            LOGGER_ERROR( "RenderEngine::renderObject2D _vertices == NULL"
            );

            return;
        }

        if( _indices == nullptr )
        {
            LOGGER_ERROR( "RenderEngine::renderObject2D _indices == NULL"
            );

            return;
        }
#endif

        if( m_renderObjects.full() == true )
        {
            LOGGER_ERROR( "RenderEngine::renderObject2D max render objects %u"
                , m_renderObjects.size()
            );

            return;
        }

        const RenderPassPtr & rp = this->requestRenderPass_( _state, _material, _vertexCount, _indexCount );

        mt::box2f bb;

        if( _bb != nullptr )
        {
            bb = *_bb;
        }
        else
        {
            Helper::makeRenderBoundingBox( bb, _vertices, _vertexCount );
        }

        const RenderCameraInterfacePtr & camera = rp->camera;
        const mt::mat4f & vpm = camera->getCameraViewProjectionMatrix();

        const RenderViewportInterfacePtr & viewport = rp->viewport;
        const Viewport & vp = viewport->getViewport();

        mt::box2f bb_homogenize;
        mt::set_box_homogenize( bb_homogenize, bb.minimum, bb.maximum, vpm );

        mt::vec2f vp_scale;
        vp.calcSize( vp_scale );

        mt::scale_box( bb_homogenize, vp_scale );
        mt::transpose_box( bb_homogenize, vp.begin );
        
        RenderMaterialInterfacePtr ro_material = _material;

        if( m_debugStepRenderMode == true && _debug == false )
        {
            if( m_iterateRenderObjects++ >= m_limitRenderObjects && m_limitRenderObjects > 0 && m_stopRenderObjects == false )
            {
                return;
            }
        }

        m_debugInfo.object += 1;

        if( m_debugFillrateCalcMode == true && _debug == false )
        {
            EPrimitiveType primitiveType = ro_material->getPrimitiveType();

            switch( primitiveType )
            {
            case PT_TRIANGLELIST:
                {
                    this->calcQuadSquare_( _vertices, _vertexCount, vp );
                }break;
            default:
                {
                }break;
            }
        }

        if( m_debugStepRenderMode == true && _debug == false )
        {
            if( m_iterateRenderObjects == m_limitRenderObjects && m_limitRenderObjects > 0 && m_stopRenderObjects == false )
            {
                RenderMaterialPtr new_material = RENDERMATERIAL_SERVICE()
                    ->getMaterial3( EM_COLOR_BLEND
                        , ro_material->getPrimitiveType()
                        , 0
                        , nullptr
                    );

                if( new_material == nullptr )
                {
                    return;
                }

                ro_material = new_material;
            }
        }

        if( m_debugRedAlertMode == true && _debug == false )
        {
            RenderMaterialInterfacePtr new_material = RENDERMATERIAL_SERVICE()
                ->getMaterial3( EM_COLOR_BLEND
                    , ro_material->getPrimitiveType()
                    , 0
                    , nullptr
                );

            if( new_material == nullptr )
            {
                return;
            }

            ro_material = new_material;
        }

        ++rp->countRenderObject;

        RenderObject & ro = m_renderObjects.emplace();

        stdex::intrusive_ptr_setup( ro.material, ro_material );

        uint32_t materialId = ro.material->getId();
        ro.materialSmartId = materialId % MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX;

        rp->materialEnd[ro.materialSmartId] = &ro;

        const RenderBatchPtr & batch = rp->batch;

        stdex::intrusive_ptr_setup( ro.indexBuffer, batch->indexBuffer );
        stdex::intrusive_ptr_setup( ro.vertexBuffer, batch->vertexBuffer );

        ro.vertexData = _vertices;
        ro.vertexCount = _vertexCount;

        ro.indexData = _indices;
        ro.indexCount = _indexCount;

        ro.bb = bb_homogenize;

        ro.minIndex = 0;
        ro.startIndex = 0;
        ro.baseVertexIndex = 0;

        ro.dipVerticesNum = 0;
        ro.dipIndiciesNum = 0;
        ro.flags = 0;

        if( _debug == true )
        {
            ro.flags |= RENDER_OBJECT_FLAG_DEBUG;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::addRenderQuad( const RenderState * _state, const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug )
    {
        uint32_t indicesNum = (_vertexCount / 4) * 6;

        if( indicesNum >= m_indicesQuad.size() )
        {
            LOGGER_ERROR( "RenderEngine::addRenderQuad count %d > max count %d"
                , indicesNum
                , m_indicesQuad.size()
            );

            return;
        }

        RenderIndex * indices = m_indicesQuad.buff();

        this->addRenderObject( _state, _material, _vertices, _vertexCount, indices, indicesNum, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::addRenderLine( const RenderState * _state, const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug )
    {
        uint32_t indicesNum = _vertexCount;

        if( indicesNum >= m_indicesLine.size() )
        {
            LOGGER_ERROR( "RenderEngine::addRenderLine count %d > max count %d"
                , indicesNum
                , m_indicesLine.size()
            );

            return;
        }

        RenderIndex * indices = m_indicesLine.buff();

        this->addRenderObject( _state, _material, _vertices, _vertexCount, indices, indicesNum, _bb, _debug );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertex2D * RenderEngine::getDebugRenderVertex2D( size_t _count )
    {
        m_debugVertices.emplace_back( TArrayRenderVertex2D() );
        TArrayRenderVertex2D & vertices_array = m_debugVertices.back();
        vertices_array.resize( _count );

        RenderVertex2D * vertices = &vertices_array.front();

        return vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setBatchMode( ERenderBatchMode _mode )
    {
        m_batchMode = _mode;
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderBatchMode RenderEngine::getBatchMode() const
    {
        return m_batchMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::makeBatches_()
    {
        for( const RenderBatchPtr & batch : m_renderBatches )
        {
            const RenderVertexBufferInterfacePtr & vertexBuffer = batch->vertexBuffer;

            vertexBuffer->resize( batch->vertexCount );

            MemoryInterfacePtr vertexMemory = vertexBuffer->lock( 0, batch->vertexCount );

            if( vertexMemory == nullptr )
            {
                LOGGER_ERROR( "failed to lock vertex buffer" );

                return false;
            }

            batch->vertexMemory = vertexMemory;

            const RenderIndexBufferInterfacePtr & indexBuffer = batch->indexBuffer;

            indexBuffer->resize( batch->indexCount );

            MemoryInterfacePtr indexMemory = indexBuffer->lock( 0, batch->indexCount );

            if( indexMemory == nullptr )
            {
                LOGGER_ERROR( "failed to lock index buffer" );

                return false;
            }

            batch->indexMemory = indexMemory;
        }

        this->insertRenderPasses_();

        for( const RenderBatchPtr & batch : m_renderBatches )
        {
            const RenderVertexBufferInterfacePtr & vertexBuffer = batch->vertexBuffer;

            if( vertexBuffer->unlock() == false )
            {
                LOGGER_ERROR( "failed to unlock vertex buffer" );

                return false;
            }

            const RenderIndexBufferInterfacePtr & indexBuffer = batch->indexBuffer;

            if( indexBuffer->unlock() == false )
            {
                LOGGER_ERROR( "failed to unlock index buffer" );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::insertRenderPasses_()
    {
        for( const RenderPassPtr & pass : m_renderPasses )
        {
            if( pass->flags & RENDER_PASS_FLAG_SINGLE )
            {
                continue;
            }

            const RenderBatchPtr & batch = pass->batch;

            uint32_t vertexSize = batch->vertexBuffer->getVertexSize();

            this->insertRenderObjects_( pass, batch->vertexMemory, vertexSize, batch->indexMemory, batch->vbPos, batch->ibPos );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::batchRenderObjectNormal_( TArrayRenderObject::iterator _begin, TArrayRenderObject::iterator _end, RenderObject * _ro, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos )
    {
        uint32_t vbPos = _vbPos;
        uint32_t ibPos = _ibPos;

        TArrayRenderObject::iterator it_batch_begin = _begin;
        ++it_batch_begin;

        RenderMaterialInterface * ro_material = _ro->material;

        for( ; it_batch_begin != _end; ++it_batch_begin )
        {
            RenderObject * ro_bath_begin = it_batch_begin;

            if( ro_bath_begin->indexCount == 0 )
            {
                continue;
            }

            RenderMaterialInterface * batch_material = ro_bath_begin->material;

            if( ro_material != batch_material )
            {
                break;
            }

            if( this->insertRenderObject_( ro_bath_begin, _vertexBuffer, _vertexSize, _indexBuffer, vbPos, ibPos ) == false )
            {
                break;
            }

            vbPos += ro_bath_begin->vertexCount;
            ibPos += ro_bath_begin->indexCount;

            _ro->dipVerticesNum += ro_bath_begin->vertexCount;
            _ro->dipIndiciesNum += ro_bath_begin->indexCount;

            stdex::intrusive_ptr_release( ro_bath_begin->material );
            stdex::intrusive_ptr_release( ro_bath_begin->vertexBuffer );
            stdex::intrusive_ptr_release( ro_bath_begin->indexBuffer );

            ro_bath_begin->dipVerticesNum = 0;
            ro_bath_begin->dipIndiciesNum = 0;
            ro_bath_begin->vertexCount = 0;
            ro_bath_begin->indexCount = 0;

            ++m_debugInfo.batch;
        }

        _vbPos = vbPos;
        _ibPos = ibPos;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_testRenderBB( const RenderObject * _begin, const RenderObject * _end, const RenderObject * _ro )
    {
        for( const RenderObject * it = _begin; it != _end; ++it )
        {
            const RenderObject * ro_bath = it;

            if( ro_bath->vertexCount == 0 )
            {
                continue;
            }

            if( mt::is_intersect( ro_bath->bb, _ro->bb ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::batchRenderObjectSmart_( const RenderPassPtr & _renderPass, TArrayRenderObject::iterator _begin, RenderObject * _ro, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos )
    {
        uint32_t vbPos = _vbPos;
        uint32_t ibPos = _ibPos;

        TArrayRenderObject::iterator it_batch_start_end = _begin;
        ++it_batch_start_end;

        TArrayRenderObject::iterator it_batch = _begin;
        ++it_batch;

        uint32_t materialSmartId = _ro->materialSmartId;
        TArrayRenderObject::const_iterator it_end = _renderPass->materialEnd[materialSmartId];

        if( _begin == it_end )
        {
            return;
        }

        const RenderMaterialInterface * ro_material = _ro->material;

        for( ; it_batch != it_end; ++it_batch )
        {
            RenderObject * ro_bath = it_batch;

            if( ro_bath->vertexCount == 0 )
            {
                continue;
            }

            const RenderMaterialInterface * ro_bath_material = ro_bath->material;

            if( ro_material != ro_bath_material )
            {
                continue;
            }

            if( s_testRenderBB( it_batch_start_end, it_batch, ro_bath ) == true )
            {
                break;
            }

            if( this->insertRenderObject_( ro_bath, _vertexBuffer, _vertexSize, _indexBuffer, vbPos, ibPos ) == false )
            {
                break;
            }

            _ro->dipVerticesNum += ro_bath->vertexCount;
            _ro->dipIndiciesNum += ro_bath->indexCount;

            stdex::intrusive_ptr_release( ro_bath->material );
            stdex::intrusive_ptr_release( ro_bath->vertexBuffer );
            stdex::intrusive_ptr_release( ro_bath->indexBuffer );

            ro_bath->dipVerticesNum = 0;
            ro_bath->dipIndiciesNum = 0;

            vbPos += ro_bath->vertexCount;
            ibPos += ro_bath->indexCount;

            ro_bath->vertexCount = 0;
            ro_bath->indexCount = 0;

            ++m_debugInfo.batch;
        }

        _vbPos = vbPos;
        _ibPos = ibPos;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::insertRenderObjects_( const RenderPassPtr & _renderPass, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos )
    {
        uint32_t vbPos = _vbPos;
        uint32_t ibPos = _ibPos;

        TArrayRenderObject::iterator it = m_renderObjects.advance( _renderPass->beginRenderObject );
        TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass->beginRenderObject + _renderPass->countRenderObject );

        for( ; it != it_end; ++it )
        {
            RenderObject * ro = it;

            if( ro->vertexCount == 0 )
            {
                continue;
            }

            ro->startIndex = ibPos;
            ro->minIndex = vbPos;

            if( this->insertRenderObject_( ro, _vertexBuffer, _vertexSize, _indexBuffer, vbPos, ibPos ) == false )
            {
                break;
            }

            ro->dipVerticesNum = ro->vertexCount;
            ro->dipIndiciesNum = ro->indexCount;

            vbPos += ro->vertexCount;
            ibPos += ro->indexCount;

            ro->vertexCount = 0;
            ro->indexCount = 0;

            switch( m_batchMode )
            {
            case ERBM_NONE:
                {
                }break;
            case ERBM_NORMAL:
                {
                    this->batchRenderObjectNormal_( it, it_end, ro, _vertexBuffer, _vertexSize, _indexBuffer, vbPos, ibPos );
                }break;
            case ERBM_SMART:
                {
                    this->batchRenderObjectNormal_( it, it_end, ro, _vertexBuffer, _vertexSize, _indexBuffer, vbPos, ibPos );

                    this->batchRenderObjectSmart_( _renderPass, it, ro, _vertexBuffer, _vertexSize, _indexBuffer, vbPos, ibPos );
                }break;
            }
        }

        _vbPos = vbPos;
        _ibPos = ibPos;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::insertRenderObject_( const RenderObject * _renderObject, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t _vbPos, uint32_t _ibPos ) const
    {
        void * memory_buffer = _vertexBuffer->getMemory();
        size_t memory_size = _vertexBuffer->getSize();

        if( stdex::memorycopy_safe( memory_buffer, _vbPos * _vertexSize, memory_size, _renderObject->vertexData, _renderObject->vertexCount * _vertexSize ) == false )
        {
            LOGGER_ERROR( "RenderEngine::insertRenderObject_ vertex buffer overrlow!"
            );

            return false;
        }

        //if( m_debugRedAlertMode == true && hasRenderObjectFlag( _renderObject, RENDER_OBJECT_FLAG_DEBUG ) == false )
        //{
        //    for( uint32_t i = 0; i != _renderObject->vertexCount; ++i )
        //    {
        //        RenderVertex2D & v = _vertexBuffer[_vbPos + i];

        //        v.color = Helper::makeARGB( 1.f, 0.f, 0.f, 0.1f );
        //    }
        //}

        RenderIndex * indexMemory = _indexBuffer->getMemory();

        RenderIndex * offsetIndicesBuffer = indexMemory + _ibPos;

        RenderIndex * src = offsetIndicesBuffer;
        RenderIndex * src_end = offsetIndicesBuffer + _renderObject->indexCount;
        const RenderIndex * dst = _renderObject->indexData;

        RenderIndex indices_offset = (RenderIndex)_vbPos;
        while( src != src_end )
        {
            *src = *dst + indices_offset;
            ++src;
            ++dst;
        }

        //_renderObject->baseVertexIndex = 0;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::flushRender_()
    {
        m_debugInfo.batch = 0;
        m_debugInfo.dips = 0;

        if( this->makeBatches_() == false )
        {
            return;
        }

        this->renderPasses_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setVSync( bool _vSync )
    {
        if( m_vsync == _vSync )
        {
            return;
        }

        m_vsync = _vSync;

        if( m_windowCreated == true )
        {
            RENDER_SYSTEM()
                ->setVSync( m_vsync );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::getVSync() const
    {
        return m_vsync;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::clear( uint32_t _color )
    {
        RENDER_SYSTEM()
            ->clear( _color, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setSeparateAlphaBlendMode()
    {
        RENDER_SYSTEM()
            ->setSeparateAlphaBlendMode();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::enableDebugFillrateCalcMode( bool _enable )
    {
        m_debugFillrateCalcMode = _enable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::isDebugFillrateCalcMode() const
    {
        return m_debugFillrateCalcMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::enableDebugStepRenderMode( bool _enable )
    {
        m_debugStepRenderMode = _enable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::isDebugStepRenderMode() const
    {
        return m_debugStepRenderMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::enableRedAlertMode( bool _enable )
    {
        m_debugRedAlertMode = _enable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::isRedAlertMode() const
    {
        return m_debugRedAlertMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::endLimitRenderObjects()
    {
        m_stopRenderObjects = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::increfLimitRenderObjects()
    {
        ++m_limitRenderObjects;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::decrefLimitRenderObjects()
    {
        if( m_limitRenderObjects == 0 )
        {
            return false;
        }

        --m_limitRenderObjects;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderEngine::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::calcQuadSquare_( const RenderVertex2D * _vertex, uint32_t _vertexNum, const Viewport & _viewport )
    {
        uint32_t triangleNum2 = (_vertexNum / 4);

        for( uint32_t i = 0; i != triangleNum2; ++i )
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

        m_debugInfo.triangle += triangleNum2 * 2;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::calcMeshSquare_( const RenderVertex2D * _vertex, uint32_t _vertexNum, const RenderIndex * _indices, uint32_t _indicesNum, const Viewport & _viewport )
    {
        (void)_vertexNum;

        for( uint32_t i = 0; i != (_indicesNum / 3); ++i )
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
