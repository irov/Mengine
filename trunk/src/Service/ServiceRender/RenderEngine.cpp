#	include "RenderEngine.h"

#	include "Logger/Logger.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ImageCodecInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "RenderSubTexture.h"

//#	include "Megatextures.h"

#   include "Interface/WatchdogInterface.h"

#   include <memory.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderService, Menge::RenderServiceInterface, Menge::RenderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine()
		: m_serviceProvider(nullptr)
		, m_windowCreated(false)
		, m_vsync(false)
		, m_vbHandle2D(0)
		, m_ibHandle2D(0)
		, m_currentVBHandle(0)
		, m_currentIBHandle(0)
		, m_currentBaseVertexIndex(0)
		, m_currentTextureStages(0)
		, m_currentRenderCamera(nullptr)        
		, m_currentRenderViewport(nullptr)
		, m_currentVertexDeclaration(0)
		, m_maxVertexCount(0)
		, m_maxIndexCount(0)
		, m_depthBufferWriteEnable(false)
		, m_alphaBlendEnable(false)
		, m_alphaTestEnable(false)
		, m_debugMode(false)
		, m_currentStage(nullptr)
		, m_nullTexture(nullptr)
		, m_debugMaterial(nullptr)
		, m_renderVertexCount(0)
		, m_renderIndicesCount(0)
		, m_batchMode(ERBM_SMART)
		, m_currentMaterialId(0)
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
	bool RenderEngine::initialize( size_t _maxVertexCount, size_t _maxIndexCount )
	{
		m_maxVertexCount = _maxVertexCount;
		m_maxIndexCount = _maxIndexCount;        

		for( uint16_t i = 0; i != MENGINE_RENDER_INDICES_QUAD; i += 6 )
		{   
			uint16_t vertexOffset = i / 6 * 4;

			m_indicesQuad[i + 0] = vertexOffset + 0;
			m_indicesQuad[i + 1] = vertexOffset + 3;
			m_indicesQuad[i + 2] = vertexOffset + 1;
			m_indicesQuad[i + 3] = vertexOffset + 1;
			m_indicesQuad[i + 4] = vertexOffset + 3;
			m_indicesQuad[i + 5] = vertexOffset + 2;
		}

		for( uint16_t i = 0; i != MENGINE_RENDER_INDICES_LINE; ++i )
		{
			m_indicesLine[i] = i;
		}

		this->setRenderSystemDefaults_();

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

		if( this->recreate2DBuffers_() == false )
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
			->createTexture( null_width, null_height, null_channels, PF_UNKNOWN );

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
		const RenderImageInterfacePtr & image = _texture->getImage();

		RENDER_SYSTEM(m_serviceProvider)
			->screenshot( image, _rect.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onRenderSystemDeviceLost()
	{
		LOGGER_WARNING(m_serviceProvider)("RenderEngine::onRenderSystemDeviceLost"
			);

		m_currentVBHandle = 0;
		m_currentIBHandle = 0;

		std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::onRenderSystemDeviceRestored()
	{
		LOGGER_WARNING(m_serviceProvider)("RenderEngine::onRenderSystemDeviceRestored"
			);

		if( this->recreate2DBuffers_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::onRenderSystemDeviceRestored invalid recreate buffers"
				);

			return false;
		}

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
		m_debugRenderVertex2D.clear();

		m_debugInfo.fillrate = 0.f;
		m_debugInfo.object = 0;
		m_debugInfo.triangle = 0;

		m_currentRenderCamera = nullptr;
		m_currentRenderViewport = nullptr;
		
		m_currentMaterialId = 0;
		m_currentStage = nullptr;

		m_renderVertexCount = 0;
		m_renderIndicesCount = 0;

		m_currentRenderTarget = m_defaultRenderTarget;

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
	void RenderEngine::updateStage_( const RenderStage * _stage )
	{
		if( m_currentStage == _stage )
		{
			return;
		}
		
		m_currentStage = _stage;

		for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			RenderTextureStage & current_texture_stage = m_currentTextureStage[stageId];
			const RenderTextureStage & texture_stage = m_currentStage->textureStage[stageId];

			if( current_texture_stage.addressU != texture_stage.addressU
				|| current_texture_stage.addressV != texture_stage.addressV )
			{
				current_texture_stage.addressU = texture_stage.addressU;
				current_texture_stage.addressV = texture_stage.addressV;

				RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( stageId
					, current_texture_stage.addressU
					, current_texture_stage.addressV 
					);
			}

			if( current_texture_stage.colorOp != texture_stage.colorOp
				|| current_texture_stage.colorArg1 != texture_stage.colorArg1
				|| current_texture_stage.colorArg2 != texture_stage.colorArg2 )
			{
				current_texture_stage.colorOp = texture_stage.colorOp;
				current_texture_stage.colorArg1 = texture_stage.colorArg1;
				current_texture_stage.colorArg2 = texture_stage.colorArg2;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( stageId
					, current_texture_stage.colorOp
					, current_texture_stage.colorArg1
					, current_texture_stage.colorArg2 
					);
			}

			if( current_texture_stage.alphaOp != texture_stage.alphaOp
				|| current_texture_stage.alphaArg1 != texture_stage.alphaArg1
				|| current_texture_stage.alphaArg2 != texture_stage.alphaArg2 )
			{
				current_texture_stage.alphaOp = texture_stage.alphaOp;
				current_texture_stage.alphaArg1 = texture_stage.alphaArg1;
				current_texture_stage.alphaArg2 = texture_stage.alphaArg2;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( stageId
					, current_texture_stage.alphaOp
					, current_texture_stage.alphaArg1
					, current_texture_stage.alphaArg2
					);
			}

			if( current_texture_stage.texCoordIndex != texture_stage.texCoordIndex )
			{
				current_texture_stage.texCoordIndex = texture_stage.texCoordIndex;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( stageId
					, current_texture_stage.texCoordIndex
					);
			}
		}

		if( m_depthBufferWriteEnable != m_currentStage->depthBufferWriteEnable )
		{
			m_depthBufferWriteEnable = m_currentStage->depthBufferWriteEnable;

			RENDER_SYSTEM(m_serviceProvider)
				->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
		}

		if( m_alphaTestEnable != m_currentStage->alphaTestEnable )
		{
			m_alphaTestEnable = m_currentStage->alphaTestEnable;

			RENDER_SYSTEM(m_serviceProvider)
				->setAlphaTestEnable( m_alphaTestEnable );
		}

		if( m_alphaBlendEnable != m_currentStage->alphaBlendEnable )
		{
			m_alphaBlendEnable = m_currentStage->alphaBlendEnable;

			RENDER_SYSTEM(m_serviceProvider)
				->setAlphaBlendEnable( m_alphaBlendEnable );
		}

		if( m_currentBlendSrc != m_currentStage->blendSrc )
		{
			m_currentBlendSrc = m_currentStage->blendSrc;

			RENDER_SYSTEM(m_serviceProvider)
				->setSrcBlendFactor( m_currentBlendSrc );
		}

		if( m_currentBlendDst != m_currentStage->blendDst )
		{
			m_currentBlendDst = m_currentStage->blendDst;

			RENDER_SYSTEM(m_serviceProvider)
				->setDstBlendFactor( m_currentBlendDst );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateMaterial_( const RenderMaterial * _material )
	{		
		if( m_currentMaterialId == _material->id )
		{
			return;
		}

		m_currentMaterialId = _material->id;

		if( m_currentTextureStages > _material->textureCount )
		{
			for( size_t stageId = _material->textureCount; stageId != m_currentTextureStages; ++stageId )
			{
				this->disableTextureStage_( stageId );
			}
		}

		m_currentTextureStages = _material->textureCount;

		for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			const RenderTextureInterface * texture = _material->textures[stageId];

			if( texture == nullptr )
			{
				const RenderTextureInterface * nullTexture = m_nullTexture.get();

				texture = nullTexture;
			}

			size_t texture_id = texture->getId();
			size_t current_texture_id = m_currentTexturesID[stageId];

			if( texture_id != current_texture_id || current_texture_id != 0 )
			{
				m_currentTexturesID[stageId] = texture_id;

				const RenderImageInterfacePtr & image = texture->getImage();

				RENDER_SYSTEM(m_serviceProvider)
					->setTexture( stageId, image );
			}
		}

		const RenderStage * stage = _material->stage;

		this->updateStage_( stage );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject_( const RenderObject* _renderObject )
	{
		if( _renderObject->dipVerticesNum == 0 )
		{
			return;
		}

		const RenderMaterial * material = _renderObject->material;

		this->updateMaterial_( material );		

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
			material->primitiveType,
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
			, stage.addressV 
			);

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
				, stage.alphaArg2 
				);

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( i
				, stage.texCoordIndex
				);

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MIPMAP, TF_LINEAR );
			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MAGNIFICATION, TF_LINEAR );
			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MINIFICATION, TF_LINEAR );

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
	void RenderEngine::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeViewMatrixLookAt( _viewMatrix, _eye, _dir, _up, _sign );
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

			RENDER_SYSTEM(m_serviceProvider)
				->setRenderTarget( nullptr, true );
		}

		float renderWidth = m_renderViewport.getWidth();
		float renderHeight = m_renderViewport.getHeight();

		//float viewportWidth = viewport.getWidth();
		//float viewportHeight = viewport.getHeight();

		size_t contentWidth = m_contentResolution.getWidth();
		size_t contentHeight = m_contentResolution.getHeight();

		//float scale_width = renderWidth / viewportWidth;
		//float scale_height = renderHeight / viewportHeight;

		float scale_width = renderWidth / float(contentWidth);
		float scale_height = renderHeight / float(contentHeight);

		const Viewport & viewport = _renderPass.viewport->getViewport();

		Viewport renderViewport;
		renderViewport.begin.x = viewport.begin.x * scale_width;
		renderViewport.begin.y = viewport.begin.y * scale_height;
		renderViewport.end.x = viewport.end.x * scale_width;
		renderViewport.end.y = viewport.end.y * scale_height;

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
	void RenderEngine::addRenderObject( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterial* _material        
		, const RenderVertex2D * _vertices, size_t _verticesNum
		, const uint16_t * _indices, size_t _indicesNum )
	{
		if( _viewport == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D viewport == NULL"
				);

			return;
		}

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

		if( _vertices == nullptr )
		{
			return;
		}

		if( _indices == nullptr )
		{
			return;
		}

		if( m_currentRenderCamera != _camera || m_currentRenderViewport != _viewport )
		{
			m_currentRenderCamera = _camera;
			m_currentRenderViewport = _viewport;

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
			pass.viewport = m_currentRenderViewport;
			pass.camera = m_currentRenderCamera;			
		}

		RenderObject & ro = m_renderObjects.emplace();

		ro.material = _material;

		ro.ibHandle = m_ibHandle2D;
		ro.vbHandle = m_vbHandle2D;

		ro.vertexData = _vertices;
		ro.verticesNum = _verticesNum;

		ro.indicesData = _indices;
		ro.indicesNum = _indicesNum;

		ro.minIndex = 0;
		ro.startIndex = 0;
		ro.baseVertexIndex = 0;

		ro.dipVerticesNum = 0;
		ro.dipIndiciesNum = 0;

		if( m_debugMode == true )
		{
			switch( _material->primitiveType )
			{
			case PT_TRIANGLELIST:
				{
					this->calcQuadSquare_( _vertices, _verticesNum );
				}break;
			}
		}

		RenderPass & rp = m_renderPasses.back();
		++rp.countRenderObject;

		m_renderVertexCount += _verticesNum;
		m_renderIndicesCount += _indicesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderQuad( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterial* _material        
		, const RenderVertex2D * _vertices, size_t _verticesNum )
	{
		size_t indicesNum = (_verticesNum / 4) * 6;

		if( indicesNum >= MENGINE_RENDER_INDICES_QUAD )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderQuad count %d > max count %d"
				, indicesNum
				, MENGINE_RENDER_INDICES_QUAD
				);

			return;
		}

		if( _material->primitiveType != PT_TRIANGLELIST )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderQuad invalid primitiveType %d"
				, _material->primitiveType
				);

			return;
		}

		this->addRenderObject( _viewport, _camera, _material, _vertices, _verticesNum, m_indicesQuad, indicesNum );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderLine( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterial* _material
		, const RenderVertex2D * _vertices, size_t _verticesNum )
	{
		size_t indicesNum = _verticesNum;

		if( indicesNum >= MENGINE_RENDER_INDICES_LINE )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderLine count %d > max count %d"
				, indicesNum
				, MENGINE_RENDER_INDICES_LINE
				);

			return;
		}

		if( _material->primitiveType != PT_LINELIST )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderLine invalid primitiveType %d"
				, _material->primitiveType
				);

			return;
		}

		this->addRenderObject( _viewport, _camera, _material, _vertices, _verticesNum, m_indicesLine, indicesNum );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setDebugMaterial( const RenderMaterial * _debugMaterial )
	{
		m_debugMaterial = _debugMaterial;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderMaterial * RenderEngine::getDebugMaterial() const
	{
		return m_debugMaterial;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertex2D * RenderEngine::getDebugRenderVertex2D( size_t _count )
	{
		if( m_debugRenderVertex2D.size() + _count > m_debugRenderVertex2D.capacity() )
		{
			return nullptr;
		}

		RenderVertex2D * vertices = m_debugRenderVertex2D.emplace_count( _count );

		return vertices;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setBatchMode( ERenderBatchMode _mode )
	{
		m_batchMode = _mode;
	}
	ERenderBatchMode RenderEngine::getBatchMode() const
	{
		return m_batchMode;
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
	IBHandle RenderEngine::createIndicesBuffer( const uint16_t * _buffer, size_t _count )
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
	bool RenderEngine::updateIndicesBuffer( IBHandle _handle, const uint16_t * _buffer, size_t _count )
	{
		void * ibuffer = RENDER_SYSTEM(m_serviceProvider)->lockIndexBuffer( _handle, 0, _count, 0 );

		if( ibuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to lock vertex buffer"
				);

			return false;
		}

		std::copy( _buffer + 0, _buffer + _count, static_cast<uint16_t*>(ibuffer) );

		if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( _handle ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to unlock vertex buffer"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::makeBatches_()
	{		
		if( m_renderVertexCount >= m_maxVertexCount )
		{
			LOGGER_WARNING(m_serviceProvider)("RenderEngine::makeBatches_: vertex buffer overflow"
				);

			return false;
		}

		if( m_renderIndicesCount >= m_maxIndexCount )
		{
			LOGGER_WARNING(m_serviceProvider)("RenderEngine::makeBatches_: indices buffer overflow"
				);

			return false;
		}

		uint32_t vbLockFlags = LOCK_DISCARD;

		void * vbData = RENDER_SYSTEM(m_serviceProvider)->lockVertexBuffer( 
			m_vbHandle2D
			, 0
			, m_renderVertexCount * sizeof(RenderVertex2D)
			, vbLockFlags 
			);

		if( vbData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock vertex buffer"
				);

			return false;
		}

		RenderVertex2D * vertexBuffer = static_cast<RenderVertex2D *>(vbData);

		uint32_t ibLockFlags = LOCK_DISCARD;
		//uint32 ibLockFlags = 0;

		void * ibData = RENDER_SYSTEM(m_serviceProvider)->lockIndexBuffer( 
			m_ibHandle2D
			, 0
			, m_renderIndicesCount * sizeof(uint16_t)
			, ibLockFlags 
			);

		if( ibData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock indices buffer"
				);

			return false;
		}

		uint16_t * indicesBuffer = static_cast<uint16_t *>(ibData);

		size_t vbInsertSize;
		size_t ibInsertSize;
		this->insertRenderPasses_( vertexBuffer, indicesBuffer, vbInsertSize, ibInsertSize );

		if( vbInsertSize != m_renderVertexCount )
		{
			LOGGER_ERROR(m_serviceProvider)("Error: failed to insert vertex buffer %d:%d"
				, vbInsertSize
				, m_renderVertexCount
				);

			return false;
		}

		if( ibInsertSize != m_renderIndicesCount )
		{
			LOGGER_ERROR(m_serviceProvider)("Error: failed to insert indices buffer %d:%d"
				, ibInsertSize
				, m_renderIndicesCount
				);

			return false;
		}

		if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( m_ibHandle2D ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Error: failed to unlock indices buffer"
				);

			return false;
		}

		if( RENDER_SYSTEM(m_serviceProvider)->unlockVertexBuffer( m_vbHandle2D ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Error: failed to unlock vertex buffer"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderPasses_( RenderVertex2D * _vertexBuffer, uint16_t * _indicesBuffer, size_t & _vbSize, size_t & _ibSize )
	{
		size_t vbPos = 0;
		size_t ibPos = 0;

		for( TArrayRenderPass::iterator 
			it = m_renderPasses.begin(), 
			it_end = m_renderPasses.end();
		it != it_end;
		++it )
		{
			RenderPass * pass = &(*it);

			this->insertRenderObjects_( pass, _vertexBuffer, _indicesBuffer, vbPos, ibPos );
		}

		_vbSize = vbPos;
		_ibSize = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static void s_setupBoxRenderObject( mt::box2f & _bx, const RenderObject * _ro )
	{
		const RenderVertex2D & v0 = _ro->vertexData[0];
		mt::reset( _bx, v0.pos.x, v0.pos.y );

		for( size_t i = 1; i != _ro->verticesNum; ++i )
		{
			const RenderVertex2D & v = _ro->vertexData[i];

			mt::add_internal_point( _bx, v.pos.x, v.pos.y );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	inline static void s_updateBoxRenderObject( mt::box2f & _bx, const RenderObject * _ro )
	{
		for( size_t i = 0; i != _ro->verticesNum; ++i )
		{
			const RenderVertex2D & v = _ro->vertexData[i];

			mt::add_internal_point( _bx, v.pos.x, v.pos.y );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderObjects_( RenderPass * _renderPass, RenderVertex2D * _vertexBuffer, uint16_t * _indicesBuffer, size_t & _vbPos, size_t & _ibPos )
	{
		TArrayRenderObject::iterator it_begin = m_renderObjects.advance( _renderPass->beginRenderObject );
		TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass->beginRenderObject + _renderPass->countRenderObject );

		size_t vbPos = _vbPos;
		size_t ibPos = _ibPos;

		for( ; it_begin != it_end; ++it_begin )
		{
			RenderObject * ro = it_begin;

			if( ro->verticesNum == 0 )
			{
				continue;
			}

			ro->startIndex = ibPos;
			ro->minIndex = vbPos;

			this->insertRenderObject_( ro, _vertexBuffer, _indicesBuffer, vbPos, ibPos );

			ro->dipVerticesNum = ro->verticesNum;
			ro->dipIndiciesNum = ro->indicesNum;

			vbPos += ro->verticesNum;
			ibPos += ro->indicesNum;

			ro->verticesNum = 0;
			ro->indicesNum = 0;

			switch( m_batchMode )
			{
			case ERBM_NONE:
				{

				}break;
			case ERBM_NORMAL:
				{
					TArrayRenderObject::iterator it_batch_begin = it_begin;
					++it_batch_begin;			

					for( ; it_batch_begin != it_end; ++it_batch_begin )
					{
						RenderObject * ro_bath_begin = it_batch_begin;

						if( ro_bath_begin->verticesNum == 0 )
						{
							continue;
						}

						if( ro->material->id != ro_bath_begin->material->id )
						{
							break;
						}

						this->insertRenderObject_( ro_bath_begin, _vertexBuffer, _indicesBuffer, vbPos, ibPos );

						ro->dipVerticesNum += ro_bath_begin->verticesNum;
						ro->dipIndiciesNum += ro_bath_begin->indicesNum;

						ro_bath_begin->dipVerticesNum = 0;
						ro_bath_begin->dipIndiciesNum = 0;

						vbPos += ro_bath_begin->verticesNum;
						ibPos += ro_bath_begin->indicesNum;

						ro_bath_begin->verticesNum = 0;
						ro_bath_begin->indicesNum = 0;

						++m_debugInfo.batch;
					}
				}break;
			case ERBM_SMART:
				{
					TArrayRenderObject::iterator it_batch_end = it_begin;
					++it_batch_end;

					TArrayRenderObject::iterator it_batch_begin = it_begin;
					++it_batch_begin;			

					mt::box2f bx;
					bool bx_initialize = false;

					for( ; it_batch_begin != it_end; ++it_batch_begin )
					{
						RenderObject * ro_bath_begin = it_batch_begin;

						if( ro_bath_begin->verticesNum == 0 )
						{
							continue;
						}
						
						if( ro->material->id == ro_bath_begin->material->id )
						{					
							if( it_batch_end != it_batch_begin )
							{			
								for( TArrayRenderObject::iterator it_batch = it_batch_end; it_batch != it_batch_begin; ++it_batch )
								{
									RenderObject * ro_bath = it_batch;

									if( ro_bath->verticesNum == 0 )
									{
										continue;
									}

									if( bx_initialize == false )
									{
										bx_initialize = true;

										s_setupBoxRenderObject( bx, ro_bath );
									}
									else
									{
										s_updateBoxRenderObject( bx, ro_bath );
									}
								}

								if( bx_initialize == true )
								{
									mt::box2f bx_batch;
									s_setupBoxRenderObject( bx_batch, ro_bath_begin );

									if( mt::is_intersect( bx_batch, bx ) == true )
									{
										break;
									}
								}
							}
							
							it_batch_end = it_batch_begin;
							++it_batch_end;

							this->insertRenderObject_( ro_bath_begin, _vertexBuffer, _indicesBuffer, vbPos, ibPos );

							ro->dipVerticesNum += ro_bath_begin->verticesNum;
							ro->dipIndiciesNum += ro_bath_begin->indicesNum;

							ro_bath_begin->dipVerticesNum = 0;
							ro_bath_begin->dipIndiciesNum = 0;

							vbPos += ro_bath_begin->verticesNum;
							ibPos += ro_bath_begin->indicesNum;

							ro_bath_begin->verticesNum = 0;
							ro_bath_begin->indicesNum = 0;

							++m_debugInfo.batch;
						}
					}

				}break;
			}
		}

		_vbPos = vbPos;
		_ibPos = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderObject_( const RenderObject * _renderObject, RenderVertex2D * _vertexBuffer, uint16_t * _indicesBuffer, size_t _vbPos, size_t _ibPos ) const
	{   
		RenderVertex2D * offsetVertexBuffer = _vertexBuffer + _vbPos;
		memcpy( offsetVertexBuffer, _renderObject->vertexData, _renderObject->verticesNum * sizeof(RenderVertex2D) );

		uint16_t * offsetIndicesBuffer = _indicesBuffer + _ibPos;

		uint16_t * src = offsetIndicesBuffer;
		uint16_t * src_end = offsetIndicesBuffer + _renderObject->indicesNum;
		const uint16_t * dst = _renderObject->indicesData;

		uint16_t indices_offset = (uint16_t)_vbPos;
		while( src != src_end )
		{
			*src = *dst + indices_offset;
			++src;
			++dst;
		}

		//_renderObject->baseVertexIndex = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::flushRender_()
	{
		m_debugInfo.batch = 0;
		if( this->makeBatches_() == false )
		{
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
	bool RenderEngine::recreate2DBuffers_()
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
			->createIndexBuffer( m_maxIndexCount, true );

		if( m_ibHandle2D == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
				, m_maxIndexCount 
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
			->createVertexBuffer( m_maxVertexCount, sizeof(RenderVertex2D), true );

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
	void RenderEngine::setRenderSystemDefaults_()
	{
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
		}

		std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );
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
	void RenderEngine::clear( uint32_t _color )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->clear( _color );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTargetTexture( const RenderTextureInterfacePtr & _texture, bool _clear )
	{
		const RenderImageInterfacePtr & image = _texture->getImage();

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

		double sq_p = sqrt(p);
		double sq_pa = sqrt(abs(p - a));
		double sq_pb = sqrt(abs(p - b));
		double sq_pc = sqrt(abs(p - c));

		double S = sq_p * sq_pa * sq_pb * sq_pc;

		return S;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::calcQuadSquare_( const RenderVertex2D * _vertex, size_t _num )
	{
		for( size_t i = 0; i != (_num / 4); ++i )
		{
			const RenderVertex2D & v0 = _vertex[i*4 + 0];
			const RenderVertex2D & v1 = _vertex[i*4 + 1];
			const RenderVertex2D & v2 = _vertex[i*4 + 2];
			const RenderVertex2D & v3 = _vertex[i*4 + 3];

			m_debugInfo.fillrate += s_calcTriangleSquare( v0, v1, v2 );
			m_debugInfo.fillrate += s_calcTriangleSquare( v0, v2, v3 );

			m_debugInfo.triangle += 2;
		}        

		m_debugInfo.object += 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::calcMeshSquare_( const RenderVertex2D * _vertex, size_t _verteNum, const uint16_t * _indices, size_t _indicesNum )
	{
		(void)_verteNum;

		for( size_t i = 0; i != (_indicesNum / 3); ++i )
		{
			uint16_t i0 = _indices[i + 0];
			uint16_t i1 = _indices[i + 1];
			uint16_t i2 = _indices[i + 2];

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
