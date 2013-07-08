#	include "RenderEngine.h"

#	include "Logger/Logger.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ImageCodecInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "RenderSubTexture.h"

//#	include "Megatextures.h"

#   include "Interface/WatchdogInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderService, Menge::RenderServiceInterface, Menge::RenderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    RenderEngine::RenderEngine()
        : m_serviceProvider(nullptr)
        , m_primitiveVertexCount(0)
        , m_windowCreated(false)
        , m_vsync(false)
        , m_vbHandle2D(0)
        , m_ibHandle2D(0)
        , m_currentVBHandle(0)
        , m_currentIBHandle(0)
        , m_currentBaseVertexIndex(0)
        , m_currentTextureStages(0)
        , m_currentRenderCamera(nullptr)        
        , m_currentVertexDeclaration(0)
        , m_maxPrimitiveVertices2D(0)
        , m_maxIndexCount(0)
        , m_vbPos(0)
        , m_depthBufferWriteEnable(false)
        , m_alphaBlendEnable(false)
        , m_alphaTestEnable(false)
        , m_textureFiltering(true)		
        //, m_renderScale(0.f, 0.f)
        //, m_renderOffset(0.f, 0.f)
        //, m_megatextures(NULL)
        , m_debugMode(false)
        , m_currentMaterial(nullptr)
        , m_nullTexture(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderEngine::~RenderEngine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
        
        RENDER_SYSTEM(m_serviceProvider)
            ->setRenderListener( this );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * RenderEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::initialize( size_t _vertexCount )
    {
        m_primitiveVertexCount = _vertexCount;

        this->setRenderSystemDefaults_( _vertexCount );

        m_debugInfo.dips = 0;
        m_debugInfo.frameCount = 0;
        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;
               
        //m_megatextures = new Megatextures(2048.f, 2048.f, PF_A8R8G8B8);

        m_defaultRenderTarget = Helper::stringizeString(m_serviceProvider, "Window");

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::finalize()
    {   
        //for( TArrayRenderObject::iterator
        //    it = m_renderObjects.begin(),
        //    it_end = m_renderObjects.end();
        //it != it_end;
        //++it )
        //{
        //    RenderObject & ro = *it;

        //    for( size_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
        //    {
        //        ro.textures[i] = nullptr;
        //    }
        //}

        m_renderObjects.clear();
        m_renderPasses.clear();
        //m_textures.clear();

        m_nullTexture = nullptr;
        
        RENDER_SYSTEM(m_serviceProvider)->releaseVertexBuffer( m_vbHandle2D );
        RENDER_SYSTEM(m_serviceProvider)->releaseIndexBuffer( m_vbHandle2D );

        for( TVectorVertexBuffer::iterator
            it = m_vertexBuffer.begin(),
            it_end = m_vertexBuffer.end();
        it != it_end;
        ++it )
        {
            VBHandle handle = *it;

            RENDER_SYSTEM(m_serviceProvider)
                ->releaseVertexBuffer( handle );
        }

        m_vertexBuffer.clear();

        for( TVectorIndexBuffer::iterator
            it = m_indexBuffer.begin(),
            it_end = m_indexBuffer.end();
        it != it_end;
        ++it )
        {
            IBHandle handle = *it;

            RENDER_SYSTEM(m_serviceProvider)
                ->releaseIndexBuffer( handle );
        }

        m_indexBuffer.clear();

        //delete m_megatextures;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, int _bits, bool _fullscreen,
        WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
    {
        m_windowResolution = _resolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;

        m_fullscreen = _fullscreen;

        LOGGER_INFO(m_serviceProvider)("RenderEngine::createRenderWindow:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
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

        m_windowCreated = RENDER_SYSTEM(m_serviceProvider)
            ->createRenderWindow( m_windowResolution, _bits, m_fullscreen, _winHandle, m_vsync, _FSAAType, _FSAAQuality );

        if( m_windowCreated == false )
        {
            return false;
        }

        if( this->createNullTexture_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::createRenderWindow invalid create null texture"
                );

            return false;
        }
        
        //m_debugInfo.megatextures = 0;

        if( this->recreate2DBuffers_( m_maxIndexCount ) == false )
        {
            return false;
        }

        this->restoreRenderSystemStates_();
        this->prepare2D_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::createNullTexture_()
    {
        size_t null_width = 4;
        size_t null_height = 4;
        size_t null_channels = 3;

        m_nullTexture = RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->createTexture( null_width, null_height, null_channels, PF_UNKNOWN, null_width, null_height );

        if( m_nullTexture == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createWhitePixelTexture_ invalid create null texture %d:%d"
                , null_width
                , null_height
                );

            return false;
        }

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = null_width;
        rect.bottom = null_height;

        int pitch = 0;
        unsigned char* textureData = m_nullTexture->lock( &pitch, rect, false );

        for( size_t it = 0; it != null_height; ++it )
        {
            unsigned char null_color = 0xFF;
            std::fill( textureData, textureData + pitch * it + null_width, null_color );
        }

        m_nullTexture->unlock();

        RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->cacheFileTexture( Helper::stringizeString(m_serviceProvider, "__null__"), m_nullTexture );

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
    void RenderEngine::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen )
    {
        m_windowResolution = _resolution;
        m_contentResolution = _contentResolution;
        m_renderViewport = _renderViewport;

        m_fullscreen = _fullscreen;

        LOGGER_INFO(m_serviceProvider)("RenderEngine::changeWindowMode:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
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

        RENDER_SYSTEM(m_serviceProvider)
            ->changeWindowMode( m_windowResolution, m_fullscreen );

        //this->restoreRenderSystemStates_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::screenshot( const RenderTextureInterfacePtr & _texture, const mt::vec4f & _rect )
    {
        RenderImageInterfacePtr image = _texture->getImage();

        RENDER_SYSTEM(m_serviceProvider)
            ->screenshot( image, _rect.buff() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::onRenderSystemDeviceRestored()
    {
        LOGGER_WARNING(m_serviceProvider)("RenderEngine::onDeviceRestored"
            );

        if( this->recreate2DBuffers_( m_maxIndexCount ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::onRenderSystemDeviceRestored invalid recreate buffers"
                );

            return false;
        }

        m_currentVBHandle = 0;
        m_currentIBHandle = 0;

        std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );

        this->restoreRenderSystemStates_();
        this->prepare2D_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::onWindowClose()
    {
        if( m_windowCreated )
        {
            RENDER_SYSTEM(m_serviceProvider)
                ->onWindowClose();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::beginScene()
    {		
        m_renderPasses.clear();
        m_renderObjects.clear();

        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;

        m_currentRenderCamera = NULL;

        m_currentRenderTarget = m_defaultRenderTarget;
        m_renderTargetResolution = m_windowResolution;

        if( RENDER_SYSTEM(m_serviceProvider)->beginScene() == false )
        {
            return false;
        }
        //m_interface->clearFrameBuffer( FBT_COLOR );
        //m_interface->setRenderViewport( m_currentRenderViewport );

        //RENDER_SYSTEM(m_serviceProvider)->setShader( m_shader );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::endScene()
    {
        this->flushRender_();

        RENDER_SYSTEM(m_serviceProvider)
            ->endScene();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::swapBuffers()
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->swapBuffers();

        m_debugInfo.frameCount += 1;		
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setRenderTarget( const ConstString& _target, bool _clear )
    {
        (void)_clear;

        m_currentRenderTarget = _target;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString& RenderEngine::getRenderTarget() const
    {
        return m_currentRenderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::isWindowCreated() const
    {
        return m_windowCreated;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::renderObject_( const RenderObject* _renderObject )
    {
        if( _renderObject->dipIndiciesNum == 0 
            || _renderObject->dipVerticesNum == 0 )
        {
            return;
        }

        if( m_currentTextureStages > _renderObject->textureStages )
        {
            for( size_t stageId = _renderObject->textureStages; stageId != m_currentTextureStages; ++stageId )
            {
                this->disableTextureStage_( stageId );
            }
        }

        const RenderMaterial* material = _renderObject->material;

        m_currentTextureStages = _renderObject->textureStages;

        for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
        {
            RenderTextureInterface * texture = _renderObject->textures[stageId];

            if( texture == nullptr )
            {
                RenderTextureInterface * nullTexture = m_nullTexture.get();

                texture = nullTexture;
            }

            size_t texture_id = texture->getId();

            if( texture_id != m_currentTexturesID[stageId] || m_currentTexturesID[stageId] != 0 )
            {
                m_currentTexturesID[stageId] = texture_id;
                RenderImageInterfacePtr image = texture->getImage();

                RENDER_SYSTEM(m_serviceProvider)
                    ->setTexture( stageId, image );
            }
        }

        if( m_currentMaterial != material )
        {
            for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
            {
                RenderTextureStage & current_stage = m_currentTextureStage[stageId];
                const RenderTextureStage & stage = material->textureStage[stageId];

                if( current_stage.filter != stage.filter )
                {
                    current_stage.filter = stage.filter;

                    RENDER_SYSTEM(m_serviceProvider)
                        ->setTextureStageFilter( stageId, TFT_MAGNIFICATION, current_stage.filter );

                    RENDER_SYSTEM(m_serviceProvider)
                        ->setTextureStageFilter( stageId, TFT_MINIFICATION, current_stage.filter );
                }

                if( current_stage.addressU != stage.addressU
                    || current_stage.addressV != stage.addressV )
                {
                    current_stage.addressU = stage.addressU;
                    current_stage.addressV = stage.addressV;

                    RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( stageId
                        , current_stage.addressU
                        , current_stage.addressV );
                }

                if( current_stage.colorOp != stage.colorOp
                    || current_stage.colorArg1 != stage.colorArg1
                    || current_stage.colorArg2 != stage.colorArg2 )
                {
                    current_stage.colorOp = stage.colorOp;
                    current_stage.colorArg1 = stage.colorArg1;
                    current_stage.colorArg2 = stage.colorArg2;

                    RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( stageId
                        , current_stage.colorOp
                        , current_stage.colorArg1
                        , current_stage.colorArg2 );
                }

                if( current_stage.alphaOp != stage.alphaOp
                    || current_stage.alphaArg1 != stage.alphaArg1
                    || current_stage.alphaArg2 != stage.alphaArg2 )
                {
                    current_stage.alphaOp = stage.alphaOp;
                    current_stage.alphaArg1 = stage.alphaArg1;
                    current_stage.alphaArg2 = stage.alphaArg2;

                    RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( stageId
                        , current_stage.alphaOp
                        , current_stage.alphaArg1
                        , current_stage.alphaArg2 );
                }

                if( current_stage.texCoordIndex != stage.texCoordIndex )
                {
                    current_stage.texCoordIndex = stage.texCoordIndex;

                    RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( stageId
                        , current_stage.texCoordIndex
                        );
                }
            }

            if( m_depthBufferWriteEnable != material->depthBufferWriteEnable )
            {
                m_depthBufferWriteEnable = material->depthBufferWriteEnable;

                RENDER_SYSTEM(m_serviceProvider)
                    ->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
            }

            if( m_alphaTestEnable != material->alphaTestEnable )
            {
                m_alphaTestEnable = material->alphaTestEnable;

                RENDER_SYSTEM(m_serviceProvider)
                    ->setAlphaTestEnable( m_alphaTestEnable );
            }

            if( m_alphaBlendEnable != material->alphaBlendEnable )
            {
                m_alphaBlendEnable = material->alphaBlendEnable;

                RENDER_SYSTEM(m_serviceProvider)
                    ->setAlphaBlendEnable( m_alphaBlendEnable );
            }

            if( m_currentBlendSrc != material->blendSrc )
            {
                m_currentBlendSrc = material->blendSrc;

                RENDER_SYSTEM(m_serviceProvider)
                    ->setSrcBlendFactor( m_currentBlendSrc );
            }

            if( m_currentBlendDst != material->blendDst )
            {
                m_currentBlendDst = material->blendDst;

                RENDER_SYSTEM(m_serviceProvider)
                    ->setDstBlendFactor( m_currentBlendDst );
            }

            m_currentMaterial = material;
        }

        if( m_currentIBHandle != _renderObject->ibHandle || 
            m_currentBaseVertexIndex != _renderObject->baseVertexIndex )
        {
            m_currentIBHandle = _renderObject->ibHandle;
            m_currentBaseVertexIndex = _renderObject->baseVertexIndex;

            RENDER_SYSTEM(m_serviceProvider)
                ->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
        }

        if( m_currentVBHandle != _renderObject->vbHandle )
        {
            m_currentVBHandle = _renderObject->vbHandle;

            RENDER_SYSTEM(m_serviceProvider)
                ->setVertexBuffer( m_currentVBHandle );
        }

        RENDER_SYSTEM(m_serviceProvider)->drawIndexedPrimitive( 
            _renderObject->primitiveType, 
            _renderObject->baseVertexIndex, 
            _renderObject->minIndex,
            _renderObject->dipVerticesNum, 
            _renderObject->startIndex, 
            _renderObject->dipIndiciesNum 
            );

        ++m_debugInfo.dips;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::disableTextureStage_( size_t _stage )
    {
        RenderTextureStage & stage = m_currentTextureStage[_stage];

        stage = RenderTextureStage();

        m_currentTexturesID[_stage] = 0;

        RENDER_SYSTEM(m_serviceProvider)
            ->setTexture( _stage, nullptr );

        RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( _stage
            , stage.addressU
            , stage.addressV );

        RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( _stage
            , stage.colorOp
            , stage.colorArg1
            , stage.colorArg2
            );

        RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( _stage
            , stage.alphaOp
            , stage.alphaArg1
            , stage.alphaArg2
            );

        RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( _stage
            , 0 
            );

        RENDER_SYSTEM(m_serviceProvider)
            ->setTextureMatrix( _stage, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::restoreRenderSystemStates_()
    {
        //mt::mat4f ident;
        m_vbPos = 0;

        mt::mat4f viewTransform;
        mt::mat4f projTransform;
        mt::mat4f worldTransform;

        mt::ident_m4( viewTransform );
        mt::ident_m4( projTransform );
        mt::ident_m4( worldTransform );

        RENDER_SYSTEM(m_serviceProvider)->setVertexBuffer( m_currentVBHandle );
        RENDER_SYSTEM(m_serviceProvider)->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
        RENDER_SYSTEM(m_serviceProvider)->setVertexDeclaration( sizeof(RenderVertex2D), Vertex2D_declaration );
        RENDER_SYSTEM(m_serviceProvider)->setProjectionMatrix( projTransform );
        RENDER_SYSTEM(m_serviceProvider)->setModelViewMatrix( viewTransform );
        RENDER_SYSTEM(m_serviceProvider)->setWorldMatrix( worldTransform );
        RENDER_SYSTEM(m_serviceProvider)->setCullMode( CM_CULL_NONE );
        RENDER_SYSTEM(m_serviceProvider)->setFillMode( FM_SOLID );
        RENDER_SYSTEM(m_serviceProvider)->setDepthBufferTestEnable( false );
        RENDER_SYSTEM(m_serviceProvider)->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
        RENDER_SYSTEM(m_serviceProvider)->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
        RENDER_SYSTEM(m_serviceProvider)->setAlphaTestEnable( m_alphaTestEnable );
        RENDER_SYSTEM(m_serviceProvider)->setAlphaBlendEnable( m_alphaBlendEnable );
        RENDER_SYSTEM(m_serviceProvider)->setAlphaCmpFunc( CMPF_GREATER_EQUAL, 0x01 );
        RENDER_SYSTEM(m_serviceProvider)->setLightingEnable( false );

        LOGGER_INFO(m_serviceProvider)("RenderEngine::restoreRenderSystemStates_ texture stages %d"
            , MENGE_MAX_TEXTURE_STAGES
            );

        for( int i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
        {
            RenderTextureStage & stage = m_currentTextureStage[i];

            RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( i, stage.addressU, stage.addressV );

            RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( i
                , stage.colorOp
                , stage.colorArg1
                , stage.colorArg2 
                );

            RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( i
                , stage.alphaOp
                , stage.alphaArg1
                , stage.alphaArg2 );

            RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( i
                , stage.texCoordIndex
                );

            RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MIPMAP, stage.filter );
            RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MAGNIFICATION, stage.filter );
            RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MINIFICATION, stage.filter );

            // skip texture matrix
            RENDER_SYSTEM(m_serviceProvider)->setTextureMatrix( i, NULL );
        }

        RENDER_SYSTEM(m_serviceProvider)->setSrcBlendFactor( m_currentBlendSrc );
        RENDER_SYSTEM(m_serviceProvider)->setDstBlendFactor( m_currentBlendDst );

        //InputStreamInterfacePtr shaderStream = FILE_SERVICE(m_serviceProvider)
        //    ->openInputFile( ConstString::none(), Helper::stringizeString( m_serviceProvider, "shader.bin" ) );

        //size_t shaderSize = shaderStream->size();

        //char * shaderBuff = new char [shaderSize];

        //shaderStream->read( shaderBuff, shaderSize );

        //RenderShaderInterface * shader = RENDER_SYSTEM(m_serviceProvider)
        //    ->createShader( shaderBuff, shaderSize );

        //m_shader = shader;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport, float zn, float zf )
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->makeProjectionOrthogonal( _projectionMatrix, _viewport, zn, zf );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf )
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->makeProjectionPerspective( _projectionMatrix, _fov, _aspect, zn, zf );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf )
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->makeProjectionFrustum( _projectionMatrix, _viewport, zn, zf );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport )
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->makeViewMatrixFromViewport( _viewMatrix, _viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _at, const mt::vec3f & _up )
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->makeViewMatrixLookAt( _viewMatrix, _eye, _at, _up );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderDebugInfo& RenderEngine::getDebugInfo() const
    {
        return m_debugInfo;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::resetFrameCount()
    {
        m_debugInfo.frameCount = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::renderPasses_()
    {
        if( m_renderPasses.empty() == true )	// nothing to render
        {
            return;
        }

        for( TArrayRenderPass::const_iterator
            it = m_renderPasses.begin(), 
            it_end = m_renderPasses.end();
        it != it_end;
        ++it )
        {
            const RenderPass & renderPass = *it;

            this->renderPass_( renderPass );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::renderPass_( const RenderPass & _renderPass )
    {
        const RenderCameraInterface * camera = _renderPass.camera;

        const ConstString& renderTarget = camera->getRenderTarget();

        if( renderTarget != m_currentRenderTarget && renderTarget.empty() == false )
        {
            m_currentRenderTarget = renderTarget;
            RENDER_SYSTEM(m_serviceProvider)->setRenderTarget( NULL, true );

            m_renderTargetResolution = m_windowResolution;
        }

        const Viewport & viewport = camera->getViewport();

        float windowWidth = (float)m_windowResolution.getWidth();
        float windowHeight = (float)m_windowResolution.getHeight();

        Viewport renderViewport;

        renderViewport.begin.x = viewport.begin.x / windowWidth;
        renderViewport.begin.y = viewport.begin.y / windowHeight;
        renderViewport.end.x = viewport.end.x / windowWidth;
        renderViewport.end.y = viewport.end.y / windowHeight;

        float renderWidth = m_renderViewport.getWidth();
        float renderHeight = m_renderViewport.getHeight();

        renderViewport.begin.x *= renderWidth;
        renderViewport.begin.y *= renderHeight;
        renderViewport.end.x *= renderWidth;
        renderViewport.end.y *= renderHeight;

        renderViewport.begin += m_renderViewport.begin;
        renderViewport.end += m_renderViewport.begin;

        //renderViewport.begin = m_renderOffset + viewport.begin * m_renderScale;
        //renderViewport.end = m_renderOffset + viewport.end * m_renderScale;

        RENDER_SYSTEM(m_serviceProvider)
            ->setViewport( renderViewport );

        const mt::mat4f & viewMatrix = camera->getViewMatrix();

        RENDER_SYSTEM(m_serviceProvider)
            ->setModelViewMatrix( viewMatrix );

        const mt::mat4f & projectionMatrix = camera->getProjectionMatrix();

        RENDER_SYSTEM(m_serviceProvider)
            ->setProjectionMatrix( projectionMatrix );

        this->renderObjects_( _renderPass );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::renderObjects_( const RenderPass & _renderPass )
    {
        TArrayRenderObject::const_iterator it_begin = m_renderObjects.advance( _renderPass.beginRenderObject );
        TArrayRenderObject::const_iterator it_end = m_renderObjects.advance( _renderPass.beginRenderObject + _renderPass.countRenderObject );

        for( ;it_begin != it_end; ++it_begin )
        {
            const RenderObject * renderObject = it_begin;

            this->renderObject_( renderObject );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::addRenderObject2D( const RenderCameraInterface * _camera, const RenderMaterial* _material, const RenderTextureInterfacePtr * _textures, size_t _texturesNum
        , ELogicPrimitiveType _type
        , const RenderVertex2D * _vertices, size_t _verticesNum
        , const uint16 * _indices, size_t _indicesNum )
    {
        if( _camera == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D camera == NULL"
                );

            return;
        }

        if( _material == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D _material == NULL"
                );

            return;
        }

        if( m_renderObjects.full() == true )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D max render objects %d"
                , MENGINE_RENDER_OBJECTS_MAX
                );

            return;
        }

        if( m_currentRenderCamera != _camera )
        {
            m_currentRenderCamera = _camera;

            if( m_renderPasses.full() == true )
            {
                LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D max render passes %d"
                    , MENGINE_RENDER_PASS_MAX
                    );

                return;
            }

            RenderPass & pass = m_renderPasses.emplace();
            pass.beginRenderObject = m_renderObjects.size();
            pass.countRenderObject = 0;
            pass.camera = m_currentRenderCamera;
        }

        RenderObject & ro = m_renderObjects.emplace();

        ro.material = _material;

        ro.textureStages = _texturesNum;

        ro.logicPrimitiveType = _type;

        ro.dipIndiciesNum = m_primitiveIndexStride[_type] * _verticesNum / m_primitiveVertexStride[_type];

        ro.ibHandle = m_ibHandle2D;
        ro.vbHandle = m_vbHandle2D;

        if( _vertices == nullptr )
        {
            return;
        }

        ro.vertexData = _vertices;
        ro.verticesNum = _verticesNum;

        ro.indicesData = _indices;
        ro.indicesNum = _indicesNum;

        ro.baseVertexIndex = 0;

        for( size_t i = 0; i != _texturesNum; ++i )
        {
            if( _textures == nullptr )
            {
                ro.textures[i] = nullptr;
            }
            else
            {
                ro.textures[i] = _textures[i].get();
            }		 
        }

        switch( _type )
        {
        case LPT_QUAD:
            //case LPT_TRIANGLE:
            ro.primitiveType = PT_TRIANGLELIST;

            if( m_debugMode == true )
            {
                this->calcQuadSquare_( _vertices, _verticesNum );
            }
            break;
        case LPT_LINE:
            //case LPT_RECTANGLE:
            ro.primitiveType = PT_LINESTRIP;
            break;
        default:
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderObject2D not support primetive type %d"
                , _type
                );

            return;            
        }

        RenderPass & rp = m_renderPasses.back();
        ++rp.countRenderObject;
    }
    //////////////////////////////////////////////////////////////////////////
    VBHandle RenderEngine::createVertexBuffer( const RenderVertex2D * _buffer, size_t _count )
    {
        VBHandle vbHandle = RENDER_SYSTEM(m_serviceProvider)
            ->createVertexBuffer( _count, sizeof(RenderVertex2D), true );

        if( vbHandle == 0 )
        {
            return 0;
        }

        m_vertexBuffer.push_back(vbHandle);

        this->updateVertexBuffer( vbHandle, _buffer, _count );

        return vbHandle;
    }
    //////////////////////////////////////////////////////////////////////////
    IBHandle RenderEngine::createIndicesBuffer( const unsigned short * _buffer, size_t _count )
    {
        IBHandle ibHandle = RENDER_SYSTEM(m_serviceProvider)
            ->createIndexBuffer( _count, false );

        if( ibHandle == 0 )
        {
            return 0;
        }

        m_indexBuffer.push_back( ibHandle );

        this->updateIndicesBuffer( ibHandle, _buffer, _count );

        return ibHandle;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::releaseVertexBuffer( VBHandle _handle )
    {
        TVectorVertexBuffer::iterator it_found = std::find( m_vertexBuffer.begin(), m_vertexBuffer.end(), _handle );

        if( it_found == m_vertexBuffer.end() )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine: failed to release vertex buffer" 
                );

            return;
        }

        m_vertexBuffer.erase( it_found );

        RENDER_SYSTEM(m_serviceProvider)
            ->releaseVertexBuffer( _handle );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::releaseIndicesBuffer( IBHandle _handle )
    {
        TVectorIndexBuffer::iterator it_found = std::find( m_indexBuffer.begin(), m_indexBuffer.end(), _handle );

        if( it_found == m_indexBuffer.end() )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine: failed to release index buffer" 
                );

            return;
        }

        m_indexBuffer.erase( it_found );

        RENDER_SYSTEM(m_serviceProvider)
            ->releaseIndexBuffer( _handle );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::updateVertexBuffer( VBHandle _handle, const RenderVertex2D * _buffer, size_t _count  )
    {
        void * vbuffer = RENDER_SYSTEM(m_serviceProvider)->lockVertexBuffer( 
            _handle
            , 0
            , _count * sizeof(RenderVertex2D)
            , 0 
            );

        if( vbuffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine: failed to lock vertex buffer"
                );

            return false;
        }

        std::copy( _buffer + 0, _buffer + _count, static_cast<RenderVertex2D*>(vbuffer) );

        if( RENDER_SYSTEM(m_serviceProvider)->unlockVertexBuffer( _handle ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine: failed to unlock vertex buffer" 
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::updateIndicesBuffer( IBHandle _handle, const unsigned short * _buffer, size_t _count )
    {
        uint16* ibuffer = RENDER_SYSTEM(m_serviceProvider)->lockIndexBuffer( _handle );

        if( ibuffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to lock vertex buffer"
                );

            return false;
        }

        std::copy( _buffer + 0, _buffer + _count, ibuffer );

        if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( _handle ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to unlock vertex buffer"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t RenderEngine::makeBatch_( size_t _offset )
    {
        size_t vbPos = _offset;

        for( TArrayRenderPass::iterator 
            it = m_renderPasses.begin(), 
            it_end = m_renderPasses.end();
        it != it_end;
        ++it )
        {
            RenderPass * pass = &(*it);

            vbPos += this->batchRenderObjects_( pass, vbPos );
        }

        return vbPos;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::makeBatches_( bool & _overflow )
    {		
        m_vbPos = 0;

        size_t vbPos = m_vbPos;
        vbPos = this->makeBatch_( vbPos );

        size_t vertexDataSize = vbPos - m_vbPos;

        if( vertexDataSize == 0 )	// nothing to render
        {
            return true;
        }

        if( vertexDataSize > m_maxPrimitiveVertices2D )
        {
            _overflow = true;

            return true;
        }

        if( vertexDataSize > m_maxPrimitiveVertices2D - m_vbPos )
        {
            m_vbPos = 0;

            vbPos = this->makeBatch_( 0 );

            vertexDataSize = vbPos;
        }

        //uint32 lockFlags = m_vbPos ? LOCK_NOOVERWRITE : LOCK_DISCARD;
        //uint32 lockFlags = LOCK_DISCARD;
        uint32 lockFlags = 0;

        void * vData = RENDER_SYSTEM(m_serviceProvider)->lockVertexBuffer( 
            m_vbHandle2D
            , 0
            , vertexDataSize * sizeof(RenderVertex2D)
            , lockFlags 
            );

        if( vData == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock vertex buffer"
                );

            return false;
        }

        size_t offset = 0;

        RenderVertex2D * vertexBuffer = static_cast<RenderVertex2D *>(vData);

        for( TArrayRenderPass::iterator 
            it = m_renderPasses.begin(), 
            it_end = m_renderPasses.end();
        it != it_end;
        ++it )
        {
            RenderPass * pass = &(*it);

            offset = this->insertRenderObjects_( pass, vertexBuffer, offset );
        }

        if( RENDER_SYSTEM(m_serviceProvider)->unlockVertexBuffer( m_vbHandle2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Error: failed to unlock vertex buffer"
                );

            return false;
        }

        _overflow = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t RenderEngine::batchRenderObjects_( RenderPass * _renderPass, size_t _startVertexPos )
    {
        RenderObject* batchedRO = NULL;
        size_t verticesNum = _startVertexPos;

        TArrayRenderObject::iterator it_begin = m_renderObjects.advance( _renderPass->beginRenderObject );
        TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass->beginRenderObject + _renderPass->countRenderObject );

        for( ; it_begin != it_end; ++it_begin )
        {
            RenderObject * renderObject = it_begin;

            if( this->batchRenderObject_( renderObject, batchedRO, verticesNum ) == false )
            {
                batchedRO = renderObject;
            }
        }

        return (verticesNum - _startVertexPos);
    }
    //////////////////////////////////////////////////////////////////////////
    inline static bool s_checkForBatch( const RenderObject * _prev, const RenderObject * _next )
    {
        if( _prev->primitiveType != PT_TRIANGLELIST )
        {
            return false;
        }

        if( _prev->textureStages != _next->textureStages )
        {
            return false;
        }

        for( size_t i = 0; i != _prev->textureStages; ++i )
        {
            RenderImageInterfacePtr prevImage = _prev->textures[i]->getImage();
            RenderImageInterfacePtr nextImage = _next->textures[i]->getImage();

            if( prevImage != nextImage )
            {
                return false;
            }
        }

        if( _prev->material != _next->material )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::batchRenderObject_( RenderObject * _renderObject, RenderObject * _batchedObject, size_t & _verticesNum ) const
    {
        size_t vertexStride = m_primitiveVertexStride[_renderObject->logicPrimitiveType];
        size_t align = (vertexStride - (_verticesNum % vertexStride)) % vertexStride;
        _verticesNum += align + _renderObject->verticesNum;

        if( _batchedObject != NULL && s_checkForBatch( _batchedObject, _renderObject ) == true )
        {
            _batchedObject->dipIndiciesNum += _renderObject->dipIndiciesNum;
            _batchedObject->dipVerticesNum += _renderObject->verticesNum;
            _renderObject->material = 0;
            _renderObject->dipVerticesNum = 0;
            _renderObject->dipIndiciesNum = 0;

            _renderObject->baseVertexIndex = 0;

            return true;
        }

        _renderObject->dipVerticesNum = _renderObject->verticesNum;
        _renderObject->baseVertexIndex = 0;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t RenderEngine::insertRenderObjects_( RenderPass * _renderPass, RenderVertex2D * _vertexBuffer, size_t _offset )
    {
        TArrayRenderObject::iterator it_begin = m_renderObjects.advance( _renderPass->beginRenderObject );
        TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass->beginRenderObject + _renderPass->countRenderObject );

        for( ; it_begin != it_end; ++it_begin )
        {
            RenderObject * ro = it_begin;

            _offset = this->insertRenderObject_( ro, _vertexBuffer, _offset );			
        }

        return _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t RenderEngine::insertRenderObject_( RenderObject * _renderObject, RenderVertex2D * _vertexBuffer, size_t _offset ) const
    {
        ELogicPrimitiveType type = _renderObject->logicPrimitiveType;

        size_t indexStart = m_primitiveIndexStart[type];
        size_t indexStride = m_primitiveIndexStride[type];
        size_t vertexStride = m_primitiveVertexStride[type];

        size_t align = ( vertexStride - ( m_vbPos % vertexStride ) ) % vertexStride;

        _offset += align;
        m_vbPos += align;

        if( m_vbPos + _renderObject->verticesNum > m_maxPrimitiveVertices2D )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::insertRenderObject_: vertex buffer overflow"
                );

            _renderObject->verticesNum = m_maxPrimitiveVertices2D - m_vbPos;
            //return;
        }

        _renderObject->startIndex  = indexStart + m_vbPos / vertexStride * indexStride;
        _renderObject->minIndex = (_renderObject->startIndex - indexStart) / indexStride * vertexStride;

        assert( _renderObject->startIndex + _renderObject->dipIndiciesNum <= m_maxIndexCount );

        //Vertex2D * vertexBuffer = _vertexBuffer + _offset * sizeof(Vertex2D)
        RenderVertex2D * offsetVertexBuffer = _vertexBuffer + _offset;
        //std::copy( _renderObject->vertexData, _renderObject->vertexData + _renderObject->verticesNum, offsetVertexBuffer );
        memcpy( offsetVertexBuffer, _renderObject->vertexData, _renderObject->verticesNum * sizeof(RenderVertex2D) );

        m_vbPos += _renderObject->verticesNum;
        _offset += _renderObject->verticesNum;

        //m_logicPrimitiveCount[type] += 1;

        return _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::flushRender_()
    {
        bool overflow = false;
        if( this->makeBatches_( overflow ) == false )
        {
            return;
        }

        if( overflow == true )
        {
            m_vbPos = 0;

            LOGGER_WARNING(m_serviceProvider)("RenderEngine::flushRender_: vertex buffer overflow"
                );

            ////vertexDataSize = m_maxPrimitiveVertices2D;
            //setRenderSystemDefaults_( m_maxQuadCount * 2 );
            //recreate2DBuffers_( m_maxIndexCount );
            //restoreRenderSystemStates_();
            //prepare2D_();
            return;
        }

        m_debugInfo.dips = 0;

        this->renderPasses_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::prepare2D_()
    {
        if( m_currentVBHandle != m_vbHandle2D )
        {
            m_currentVBHandle = m_vbHandle2D;

            RENDER_SYSTEM(m_serviceProvider)
                ->setVertexBuffer( m_currentVBHandle );
        }

        if( m_currentVertexDeclaration != Vertex2D_declaration )
        {
            m_currentVertexDeclaration = Vertex2D_declaration;

            RENDER_SYSTEM(m_serviceProvider)
                ->setVertexDeclaration( sizeof(RenderVertex2D), m_currentVertexDeclaration );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::prepare3D_()
    {		
        if( m_currentVertexDeclaration != Vertex3D_declaration )
        {
            m_currentVertexDeclaration = Vertex3D_declaration;

            RENDER_SYSTEM(m_serviceProvider)
                ->setVertexDeclaration( sizeof(RenderVertex3D), m_currentVertexDeclaration );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::refillIndexBuffer2D_( size_t & _maxVertices )
    {
        uint16* ibuffer = RENDER_SYSTEM(m_serviceProvider)
            ->lockIndexBuffer( m_ibHandle2D );

        // s
        uint16 vertexCount = 0;
        uint16 maxVertices = 0;
        for( size_t i = m_primitiveIndexStart[LPT_QUAD];
            i + m_primitiveIndexStride[LPT_QUAD] < m_primitiveIndexStart[LPT_LINE];
            i += 6, vertexCount += 4 )
        {
            ibuffer[i+0] = 0 + vertexCount;
            ibuffer[i+1] = 3 + vertexCount;
            ibuffer[i+2] = 1 + vertexCount;
            ibuffer[i+3] = 1 + vertexCount;
            ibuffer[i+4] = 3 + vertexCount;
            ibuffer[i+5] = 2 + vertexCount;
        }
        maxVertices = vertexCount;
        // TRIANGLES
        //vertexCount = 0;
        //for( size_t i = m_primitiveIndexStart[LPT_TRIANGLE];
        //	i + m_primitiveIndexStride[LPT_TRIANGLE] < m_primitiveIndexStart[LPT_LINE];
        //	i += 3, vertexCount += 3 )
        //{
        //	ibuffer[i+0] = 0 + vertexCount;
        //	ibuffer[i+1] = 1 + vertexCount;
        //	ibuffer[i+2] = 2 + vertexCount;
        //}
        maxVertices = (std::min)( maxVertices, vertexCount );
        // LINES
        vertexCount = 0;
        for( size_t i = m_primitiveIndexStart[LPT_LINE];
            i + m_primitiveIndexStride[LPT_LINE] < m_maxIndexCount;
            i += 1, vertexCount += 1 )
        {
            ibuffer[i+0] = 0 + vertexCount;
        }

        maxVertices = (std::min)( maxVertices, vertexCount );
        // RECTANGLES
        //vertexCount = 0;
        //for( size_t i = m_primitiveIndexStart[LPT_RECTANGLE];
        //	i + m_primitiveIndexStride[LPT_RECTANGLE] < m_maxIndexCount;
        //	i += 5, vertexCount += 4 )
        //{
        //	ibuffer[i+0] = 0 + vertexCount;
        //	ibuffer[i+1] = 1 + vertexCount;
        //	ibuffer[i+2] = 2 + vertexCount;
        //	ibuffer[i+3] = 3 + vertexCount;
        //	ibuffer[i+4] = 0 + vertexCount;
        //}

        maxVertices = (std::min)( maxVertices, vertexCount );

        if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( m_ibHandle2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::refillIndexBuffer2D_: failed to unlock index buffer"
                );

            return false;
        }

        _maxVertices = maxVertices;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::recreate2DBuffers_( size_t _maxIndexCount )
    {
        if( m_ibHandle2D != 0 )
        {
            RENDER_SYSTEM(m_serviceProvider)
                ->releaseIndexBuffer( m_ibHandle2D );

            if( m_currentIBHandle == m_ibHandle2D )
            {
                m_currentIBHandle = 0;
            }

            m_ibHandle2D = 0;
        }

        m_ibHandle2D = RENDER_SYSTEM(m_serviceProvider)
            ->createIndexBuffer( _maxIndexCount, false );

        if( m_ibHandle2D == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
                , m_maxIndexCount 
                );

            return false;
        }

        if( this->refillIndexBuffer2D_( m_maxPrimitiveVertices2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::recreate2DBuffers_: can't refill index buffer"
                );

            return false;
        }

        if( m_vbHandle2D != 0 )
        {
            RENDER_SYSTEM(m_serviceProvider)
                ->releaseVertexBuffer( m_vbHandle2D );

            if( m_currentVBHandle == m_vbHandle2D )
            {
                m_currentVBHandle = 0;
            }

            m_vbHandle2D = 0;
        }

        m_vbHandle2D = RENDER_SYSTEM(m_serviceProvider)
            ->createVertexBuffer( m_maxPrimitiveVertices2D, sizeof(RenderVertex2D), true );

        if( m_vbHandle2D == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
                , m_maxIndexCount 
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setRenderSystemDefaults_( size_t _vertexCount )
    {
        m_primitiveIndexStride[LPT_QUAD] = 6;
        m_primitiveVertexStride[LPT_QUAD] = 4;
        //m_primitiveIndexStride[LPT_TRIANGLE] = 3;
        //m_primitiveVertexStride[LPT_TRIANGLE] = 3;
        m_primitiveIndexStride[LPT_LINE] = 1;
        m_primitiveVertexStride[LPT_LINE] = 1;
        //m_primitiveIndexStride[LPT_RECTANGLE] = 5;
        //m_primitiveVertexStride[LPT_RECTANGLE] = 4;

        for( size_t i = 0; i != LPT_PRIMITIVE_COUNT; ++i )
        {
            m_primitiveCount[i] = _vertexCount / m_primitiveVertexStride[i];
        }

        m_primitiveIndexStart[LPT_QUAD] = 0;
        for( size_t i = 0; i != LPT_PRIMITIVE_COUNT - 1; ++i )
        {
            m_primitiveIndexStart[i + 1] = m_primitiveIndexStart[i] + m_primitiveCount[i] * m_primitiveIndexStride[i];
        }

        m_maxIndexCount = 0;
        for( size_t i = 0; i != LPT_PRIMITIVE_COUNT; ++i )
        {
            m_maxIndexCount += m_primitiveCount[i] * m_primitiveIndexStride[i];
        }

        m_currentBlendSrc = BF_ONE;
        m_currentBlendDst = BF_ZERO;
        m_depthBufferWriteEnable = false;
        m_alphaBlendEnable = false;
        m_alphaTestEnable = false;

        for( int i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
        {
            RenderTextureStage & stage = m_currentTextureStage[i];

            stage.addressU = TAM_CLAMP;
            stage.addressV = TAM_CLAMP;

            stage.colorOp = TOP_DISABLE;
            stage.colorArg1 = TARG_TEXTURE;
            stage.colorArg2 = TARG_DIFFUSE;

            stage.alphaOp = TOP_DISABLE;
            stage.colorArg1 = TARG_TEXTURE;
            stage.colorArg2 = TARG_DIFFUSE;

            stage.filter = TF_LINEAR;
        }

        std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::enableTextureFiltering( bool _enable )
    {
        m_textureFiltering = _enable;

        //for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
        //{
        //	ETextureFilter tFilter = m_textureFiltering ? TF_LINEAR : TF_NONE;

        //	m_interface->setTextureStageFilter( i, TFT_MIPMAP, TF_NONE );
        //	m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, tFilter );
        //	m_interface->setTextureStageFilter( i, TFT_MINIFICATION, tFilter );
        //}
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::isTextureFilteringEnabled() const
    {
        return m_textureFiltering;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::isResolutionAppropriate() const
    {
        return m_renderTargetResolution == m_contentResolution;
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
            RENDER_SYSTEM(m_serviceProvider)
                ->setVSync( m_vsync );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::getVSync() const
    {
        return m_vsync;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::clear( uint32 _color )
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->clear( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setRenderTargetTexture( const RenderTextureInterfacePtr & _texture, bool _clear )
    {
        RenderImageInterfacePtr image = _texture->getImage();

        RENDER_SYSTEM(m_serviceProvider)
            ->setRenderTarget( image, _clear );

        this->restoreRenderSystemStates_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setSeparateAlphaBlendMode()
    {
        RENDER_SYSTEM(m_serviceProvider)
            ->setSeparateAlphaBlendMode();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::enableDebugMode( bool _enable )
    {
        m_debugMode = _enable;
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderEngine::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    static double s_calcTriangleSquare( const RenderVertex2D & _v1, const RenderVertex2D & _v2, const RenderVertex2D & _v3 )
    {
        const mt::vec3f & p1 = _v1.pos;
        const mt::vec3f & p2 = _v2.pos;
        const mt::vec3f & p3 = _v3.pos;

        double a = (double)mt::length_v3_v3( p1, p2 );
        double b = (double)mt::length_v3_v3( p2, p3 );
        double c = (double)mt::length_v3_v3( p3, p1 );

        double p = (a + b + c) * 0.5;

        double S2 = p * abs(p-a) * abs(p-b) * abs(p-c);

        double S = sqrt( S2 );

        return S;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::calcQuadSquare_( const RenderVertex2D * _vertex, size_t _num )
    {
        for( size_t i = 0; i != (_num / 4); ++i )
        {
            const RenderVertex2D & v0 = _vertex[i + 0];
            const RenderVertex2D & v1 = _vertex[i + 1];
            const RenderVertex2D & v2 = _vertex[i + 2];
            const RenderVertex2D & v3 = _vertex[i + 3];

            m_debugInfo.fillrate += s_calcTriangleSquare( v0, v1, v2 );
            m_debugInfo.fillrate += s_calcTriangleSquare( v0, v2, v3 );

            m_debugInfo.triangle += 2;
        }        

        m_debugInfo.object += 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::calcMeshSquare_( const RenderVertex2D * _vertex, size_t _verteNum, const uint16 * _indices, size_t _indicesNum )
    {
        for( size_t i = 0; i != (_indicesNum / 3); ++i )
        {
            uint16 i0 = _indices[i + 0];
            uint16 i1 = _indices[i + 1];
            uint16 i2 = _indices[i + 2];

            m_debugInfo.fillrate += s_calcTriangleSquare( _vertex[i0], _vertex[i1], _vertex[i2] );

            m_debugInfo.triangle += 1;
        }

        m_debugInfo.object += 1;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t RenderEngine::getMemorySize( size_t _width, size_t _height, size_t _depth, PixelFormat _format ) const
    {
        switch( _format )
        {
        case PF_DXT1:
            return ((_width + 3) / 4) * ( (_height + 3) / 4 ) * 8;
        case PF_DXT2:
        case PF_DXT3:
        case PF_DXT4:
        case PF_DXT5:
            return ((_width + 3) / 4) * ( (_height + 3) / 4) * 16;
        case PF_L8:
        case PF_A8:
            return _width * _height * _depth * 1;
        case PF_R8G8B8:
        case PF_B8G8R8:
            return _width * _height * _depth * 3;
        case PF_A8R8G8B8:
        case PF_A8B8G8R8:
        case PF_B8G8R8A8:
        case PF_R8G8B8A8:
        case PF_X8R8G8B8:
        case PF_X8B8G8R8:
            return _width * _height * _depth * 4;
        }                

        return 0;
    }
}
