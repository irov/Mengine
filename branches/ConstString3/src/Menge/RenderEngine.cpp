#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlEngine.h"

#	include "Logger/Logger.h"

#	include "Application.h"
#	include "Game.h"

#	include "CodecEngine.h"
#	include "Interface/ImageCodecInterface.h"

#	include "ResourceTexture.h"
#	include "ResourceImage.h"
#	include "Material.h"

#	include "Camera.h"

#	include "Texture.h"
#	include "Vertex.h"

#	include "Core/PixelFormat.h"

#	include "Consts.h"

#	include <algorithm>
#	include <map>
#	include <ctime>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static bool s_cmpMaterials( const Material* _left, const Material* _right, int _textureStages )
	{
		if( _left->blendSrc != _right->blendSrc
			|| _left->blendDst != _right->blendDst )
		{
			return false;
		}

		for( int i = 0; i < _textureStages; ++i )
		{
			const TextureStage & leftTS = _left->textureStage[i];
			const TextureStage & rightTS = _right->textureStage[i];

			if( leftTS.addressU != rightTS.addressU
				|| leftTS.addressV != rightTS.addressV
				|| leftTS.colorOp != rightTS.colorOp
				|| leftTS.colorArg1 != rightTS.colorArg1
				|| leftTS.colorArg2 != rightTS.colorArg2
				|| leftTS.alphaOp != rightTS.alphaOp
				|| leftTS.alphaArg1 != rightTS.alphaArg1
				|| leftTS.alphaArg2 != rightTS.alphaArg2 )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( FactoryIdentity * _factoryIdentity)
		: m_interface( NULL )
		, m_factoryIdentity(_factoryIdentity)
		, m_windowCreated( false )
		, m_vsync( false )
		, m_layer3D( false )
		, m_vbHandle2D( 0 )
		, m_ibHandle2D( 0 )
		, m_batchedObject( NULL )
		, m_currentVBHandle( 0 )
		, m_currentIBHandle( 0 )
		, m_currentTextureStages( 0 )
		, m_activeCamera( NULL )
		, m_nullTexture( NULL )
		, m_currentVertexDeclaration( 0 )
		, m_maxIndexCount(0)
		, m_depthBufferWriteEnable( false )
		, m_alphaBlendEnable( false )
		, m_alphaTestEnable( false )
		, m_textureFiltering( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::~RenderEngine()
	{
		for( TTextureMap::iterator 
			it = m_textures.begin(), 
			it_end = m_textures.end();
		it != it_end;
		++it )
		{
			this->destroyTexture( it->second );
		}

		m_textures.clear();

		if( m_interface != NULL )
		{
			m_interface->releaseVertexBuffer( m_vbHandle2D );
			m_interface->releaseIndexBuffer( m_vbHandle2D );
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize( int _maxQuadCount )
	{
		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false ) || ( m_interface == NULL ) )
		{
			return false;
		}

		setRenderSystemDefaults_( _maxQuadCount );

		LogSystemInterface * system = Logger::hostage()->getInterface();

		if( m_interface->initialize( system, this ) == false )
		{
			return false;
		}
		//m_interface->setEventListener( this );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen,
		WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
	{
		m_windowResolution = _resolution;
		m_fullscreen = _fullscreen;

		m_windowCreated = m_interface->createRenderWindow( _resolution[0], _resolution[1], _bits, _fullscreen, _winHandle,
			m_vsync, _FSAAType, _FSAAQuality );

		if( m_windowCreated == false )
		{
			return false;
		}

		m_debugInfo.textureMemory = 0;
		m_debugInfo.frameCount = 0;

		//////////////////////////////////////////////////////////////////////////
		// ¬ыноси такое в отдельные функции, читать невозможно
		//////////////////////////////////////////////////////////////////////////

		m_nullTexture = createTexture( Consts::c_NullTexture, 2, 2, PF_R8G8B8 );

		int pitch = 0;
		unsigned char* textureData = m_nullTexture->lock( &pitch, false );
		std::fill( textureData, textureData + pitch * 2, 0xFF );
		m_nullTexture->unlock();

		m_currentRenderTarget = Consts::c_Window;

		if( recreate2DBuffers_( m_maxIndexCount ) == false )
		{
			return false;
		}

		m_vbPos = 0;

		//m_vbHandle3D = m_interface->createVertexBuffer( c_vertexCount3D, sizeof( Vertex3D ) );
		//m_ibHandle3D = m_interface->createIndexBuffer( c_vertexCount3D );

		//m_interface->setProjectionMatrix( m_projTranfsorm2D.buff() );

		//////////////////////////////////////////////////////////////////////////
		// € воткнул это в setRenderSystemDefaults_
		//

		// мне кажетс€ что это тоже надо воткнуть туда. и вообще если по умолчанию colorOp = TOP_DISABLE
		// то может это вынести в конструктор?
		//for( int i = 1; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		//{
		//	m_currentTextureStage[i].colorOp = TOP_DISABLE;
		//}

		restoreRenderSystemStates_();
		prepare2D_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( Texture* _image, const mt::vec4f & _rect )
	{
		RenderImageInterface* iInterface = _image->getInterface();

		m_interface->screenshot( iInterface, _rect.buff() );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer2D()
	{
		if( m_layer3D == true )
		{
			prepare3D_();
			m_interface->clearFrameBuffer( FBT_DEPTH );
			m_layer3D = false;
			flushRender_();
			m_interface->beginLayer2D();
		}
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLayer2D()
	{
		m_interface->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer3D()
	{
		/*if( m_layer3D == false )
		{
			prepare2D_();
			m_interface->clearFrameBuffer( FBT_DEPTH );
			m_layer3D = true;
			flushRender_();
			m_interface->beginLayer3D();
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::setProjectionMatrix( const mt::mat4f& _projection )
	//{
	//	RenderObject* ro = getTempRenderObject_();
	//	ro->setProjTransform = true;
	//	ro->projTransform = _projection;
	//	renderObject( ro );
	//}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::setViewMatrix( const mt::mat4f& _view )
	//{
	//	RenderObject* ro = getTempRenderObject_();
	//	ro->setViewTransform = true;
	//	ro->viewTransform = _view;
	//	renderObject( ro );
	//}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	//{
	//	RenderObject* ro = getTempRenderObject_();
	//	ro->setWorldTransform = true;
	//	ro->worldTransform = _world;
	//	renderObject( ro );
	//}	
	//////////////////////////////////////////////////////////////////////////
	Material * RenderEngine::createMaterial()
	{
		Material * material = m_renderMaterialPool.get();

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseMaterial( Material* _material )
	{
		m_renderMaterialPool.release( _material );
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* RenderEngine::createTexture( const ConstString & _name, size_t _width, size_t _height, PixelFormat _format )
	{
		TTextureMap::iterator it_find = m_textures.find( _name );

		if( it_find != m_textures.end() )
		{
			MENGE_LOG_WARNING( "Warning: (RenderEngine::createImage) Image '%s' already exist"
				, _name.c_str()
				);

			return it_find->second;
		}

		size_t hwWidth = _width;
		size_t hwHeight = _height;
		PixelFormat hwPixelFormat = _format;
		RenderImageInterface* image = m_interface->createImage( hwWidth, hwHeight, hwPixelFormat );

		if( image == NULL )
		{
			MENGE_LOG_ERROR( "Error: (RenderEngine::createImage) RenderSystem couldn't create image '%s' %dx%d"
				, _name.c_str()
				, _width
				, _height 
				);

			return NULL;
		}
		m_debugInfo.textureMemory += PixelUtil::getMemorySize( _width, _height, 1, _format );

		Texture* texture = new Texture( image, _name, _width, _height, _format, hwWidth, hwHeight, hwPixelFormat );
		m_textures.insert( std::make_pair( _name, texture ) );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* RenderEngine::createRenderTargetTexture( const ConstString & _name, const mt::vec2f & _resolution )
	{
		TTextureMap::iterator it_find = m_renderTargets.find( _name );
		if( it_find != m_renderTargets.end() )
		{
			MENGE_LOG_WARNING( "Warning: (RenderEngine::createRenderTargetImage) RenderTarget '%s' already exist"
				, _name.c_str() 
				);

			return it_find->second;
		}

		size_t width = ::floorf( _resolution.x + 0.5f );
		size_t height = ::floorf( _resolution.y + 0.5f );
		size_t hwWidth = width;
		size_t hwHeight = height;
		RenderImageInterface * image = m_interface->createRenderTargetImage( hwWidth, hwHeight );

		if( image == NULL )
		{
			MENGE_LOG_ERROR( "Error: (RenderEngine::createRenderTargetImage) RenderSystem couldn't create RenderTarget '%s' %dx%d"
				, _name.c_str()
				, width
				, height 
				);

			return NULL;
		}

		m_debugInfo.textureMemory += PixelUtil::getMemorySize( width, height, 1, PF_A8R8G8B8 );

		Texture* texture = new Texture( image, _name, width, height, PF_A8R8G8B8, hwWidth, hwHeight, PF_A8R8G8B8 );
		m_renderTargets.insert( std::make_pair( _name, texture ) );
		m_textures.insert( std::make_pair( _name, texture ) );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::saveImage( Texture* _image, const ConstString & _fileSystemName, const String & _filename )
	{
		ImageEncoderInterface * imageEncoder = CodecEngine::hostage()
			->createEncoderT<ImageEncoderInterface>( _fileSystemName, _filename, Consts::c_Image );

		if( imageEncoder == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : can't create encoder for filename '%s'"
				, _filename.c_str() 
				);

			return false;
		}

		ImageCodecDataInfo dataInfo;
		dataInfo.format = _image->getHWPixelFormat();
		dataInfo.height = _image->getHeight();
		dataInfo.width = _image->getWidth();
		dataInfo.depth = 1;
		dataInfo.num_mipmaps = 0;
		dataInfo.flags = 0;
		dataInfo.size = 0;	// we don't need this
		int pitch = 0;
		unsigned char* buffer = _image->lock( &pitch );

		/*unsigned char* lockBuffer = new unsigned char[ imgData.height * pitch ];

		std::size_t mPitch = Menge::PixelUtil::getNumElemBytes( imgData.format ) * imgData.width;
		for( std::size_t i = 0; i != imgData.height; i++ )
		{
		std::copy( buffer, buffer + mPitch, lockBuffer );
		//memcpy( _dstData, _srcData, width * 4 );
		lockBuffer += mPitch;
		buffer += pitch;
		}
		_image->unlock();*/

		//lockBuffer -= mPitch * imgData.height;

		unsigned int encoderOptions = 0;
		encoderOptions |= DF_CUSTOM_PITCH;
		encoderOptions |= (pitch << 16);

		imageEncoder->setOptions( encoderOptions );
		unsigned int bytesWritten = imageEncoder->encode( buffer, &dataInfo );

		_image->unlock();

		Holder<CodecEngine>::hostage()
			->releaseEncoder( imageEncoder );

		if( bytesWritten == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : Error while encoding image data" );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* RenderEngine::loadTexture( const ConstString& _pakName, const ConstString & _filename )
	{
		//RenderImageInterface * image = m_interface->getImage( _filename );
		Texture* rTexture = NULL;
		TTextureMap::iterator it_find = m_textures.find( _filename );
		if( it_find != m_textures.end() )
		{
			it_find->second->addRef();
			rTexture = it_find->second;
		}
		else
		{
			ImageDecoderInterface * imageDecoder = CodecEngine::hostage()
				->createDecoderT<ImageDecoderInterface>( _pakName, _filename.str(), Consts::c_Image );

			if( imageDecoder == 0 )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file '%s' was not found"
					, _filename.c_str() 
					);

				return NULL;
			}

			const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

			if( dataInfo->format == PF_UNKNOWN )
			{
				MENGE_LOG_ERROR( "Error: Invalid image format '%s'"
					, _filename.c_str() 
					);

				Holder<CodecEngine>::hostage()
					->releaseDecoder( imageDecoder );

				return NULL;
			}

			rTexture = createTexture( _filename, dataInfo->width, dataInfo->height, dataInfo->format );
			if( rTexture == NULL )
			{
				Holder<CodecEngine>::hostage()
					->releaseDecoder( imageDecoder );
				return NULL;
			}

			rTexture->loadImageData( imageDecoder );

			Holder<CodecEngine>::hostage()
				->releaseDecoder( imageDecoder );

			m_textures.insert( std::make_pair( _filename, rTexture ) );

			//MemoryTextureProfiler::addMemTexture( _filename, dataInfo->size );
		}

		return rTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseTexture( Texture* _texture )
	{
		if( _texture == NULL )
		{
			return;
		}

		if( _texture->decRef() == 0 )
		{
			const ConstString & name = _texture->getName();
			m_textures.erase( name );

			this->destroyTexture( _texture );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::destroyTexture( Texture* _texture )
	{
		RenderImageInterface* image = _texture->getInterface();

		size_t width = _texture->getWidth();
		size_t height = _texture->getHeight();

		PixelFormat format = _texture->getPixelFormat();

		m_debugInfo.textureMemory -= PixelUtil::getMemorySize( width, height, 1, format );

		m_interface->releaseImage( image );

		const ConstString & name = _texture->getName();
		m_renderTargets.erase( name );

		delete _texture;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;
		m_fullscreen = _fullscreen;

		if( m_windowCreated == false )
		{
			return;
		}

		m_interface->changeWindowMode( _resolution, _fullscreen );

		restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * RenderEngine::createLight( const ConstString& _name )
	{
		return m_interface->createLight( _name.str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseLight( LightInterface * _light )
	{
		return m_interface->releaseLight( _light );
	}
	//////////////////////////////////////////////////////////////////////////
	Resolution RenderEngine::getBestDisplayResolution( const Resolution & _resolution, float _aspect )
	{
		TVectorResolutions resolutions;
		m_interface->getResolutions( resolutions );

		float rx = float(_resolution[0]);
		float ry = float(_resolution[1]);

		float needWidth = ry * _aspect;

		typedef std::map<int, TVectorResolutions> TResolutionMap;

		TResolutionMap resMap;

		std::size_t count = resolutions.size() / 2;

		for( std::size_t i = 0; i < count; i++ )
		{
			if( fabsf( ( static_cast<float>( resolutions[2*i] ) / resolutions[2*i+1] ) - _aspect ) < 0.01f )
			{
				resMap[ resolutions[2*i + 1] ].push_back( resolutions[2*i] );
			}
		}

		bool done = false;

		Resolution bestResolution;

		for( TResolutionMap::iterator 
			it = resMap.begin(),
			it_end = resMap.end();
		it != it_end; 
		++it )
		{
			if( it->first < static_cast<int>(_resolution[1]) ) 
			{
				continue;
			}

			for( TVectorResolutions::size_type 
				i = 0,
				i_end = it->second.size();
			i != i_end; 
			++i)
			{
				if( it->second[i] >= static_cast<int>(needWidth) )
				{
					bestResolution.setWidth( it->second[i] );
					bestResolution.setHeight( it->first );
					done = true;
					break;
				}
			}
			if( done ) break;
		}

		return bestResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onDeviceRestored()
	{
		//const Resolution & resolution = Holder<Game>::hostage()
		//	->getResolution();

		//Holder<Application>::hostage()
		//	->notifyWindowModeChanged( resolution[0], resolution[1], m_fullscreen );
		refillIndexBuffer2D_();
		restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	/*void RenderEngine::onWindowActive( bool _active )
	{
	if( m_windowCreated )
	{
	m_interface->onWindowActive( _active );
	}
	}*/
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowClose()
	{
		if( m_windowCreated )
		{
			m_interface->onWindowClose();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::beginScene()
	{
		m_cameras.clear();

		m_activeCamera = NULL;

		m_layerZ = 1.0f;
		m_currentRenderTarget = Consts::c_Window;
		m_renderTargetResolution = m_windowResolution;
		m_dipCount = 0;
		if( m_interface->beginScene() == false )
		{
			return false;
		}
		m_interface->clearFrameBuffer( FBT_COLOR | FBT_DEPTH );
		m_currentRenderViewport = m_renderViewport;
		m_interface->setRenderViewport( m_currentRenderViewport );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		flushRender_();
		m_interface->endScene();
		m_debugInfo.dips = m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::swapBuffers()
	{
		m_interface->swapBuffers();
		m_debugInfo.frameCount += 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::applyRenderViewport( const Viewport & _renderViewport )
	{
		const Resolution & contentResolution = 
			Game::hostage()->getContentResolution();

		float rx = m_currentRenderViewport.getWidth() / static_cast<float>( contentResolution[0] );
		float ry = m_currentRenderViewport.getHeight() / static_cast<float>( contentResolution[1] );
		float dx = 0.0f;
		float dy = 0.0f;

		Viewport renderViewport = _renderViewport;
		Viewport projectionViewport = _renderViewport;

		if( renderViewport.begin.x < 0.001f
			&& renderViewport.begin.y < 0.001f
			&& renderViewport.end.x < 0.001f
			&& renderViewport.end.y < 0.001f )
		{
			renderViewport = m_currentRenderViewport;
			projectionViewport.end.x = contentResolution[0];
			projectionViewport.end.y = contentResolution[1];
		}
		else
		{
			renderViewport.begin.x = renderViewport.begin.x * rx;
			renderViewport.begin.y = renderViewport.begin.y * ry;
			renderViewport.end.x = renderViewport.end.x * rx;
			renderViewport.end.y = renderViewport.end.y * ry;
			renderViewport.begin += m_currentRenderViewport.begin;
			renderViewport.end += m_currentRenderViewport.begin;
		}

		setProjectionMatrix2D_( m_projTransform, projectionViewport.begin.x, projectionViewport.end.x,
			projectionViewport.begin.y, projectionViewport.end.y, 0.0f, 1.0f );

		m_interface->setProjectionMatrix( m_projTransform.buff() );
		m_interface->setRenderViewport( renderViewport );

	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const ConstString& _target, bool _clear )
	{
		m_currentRenderTarget = _target;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& RenderEngine::getRenderTarget() const
	{
		return m_currentRenderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderViewport( const Viewport & _viewport )
	{
		m_renderViewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & RenderEngine::getRenderViewport() const
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f& RenderEngine::getViewTransform() const
	{
		return m_viewTransform;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isWindowCreated() const
	{
		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::checkForBatch_( RenderObject* _prev, RenderObject* _next )
	{
		if( _prev == NULL || _next == NULL )
		{
			return false;
		}

		if( _prev->primitiveType == PT_LINESTRIP		// this primitives could'n be batched
		|| _prev->primitiveType == PT_TRIANGLESTRIP 
		|| _prev->primitiveType == PT_TRIANGLEFAN 
		|| _prev->textureStages != _next->textureStages )
		{
			return false;
		}

		for( std::size_t i = 0; i != _prev->textureStages; ++i )
		{
			if( _prev->textures[i] != _next->textures[i] )
			{
				return false;
			}
		}

		return s_cmpMaterials( _prev->material, _next->material, _prev->textureStages );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( RenderObject* _renderObject )
	{
		Material* _pass = _renderObject->material;

		if( m_currentTextureStages > _renderObject->textureStages )
		{
			for( std::size_t i = _renderObject->textureStages; i < m_currentTextureStages; ++i )
			{
				enableTextureStage_( i, false );
			}
			m_currentTextureStages = _renderObject->textureStages;
		}
		else if( m_currentTextureStages < _renderObject->textureStages )
		{
			m_currentTextureStages = _renderObject->textureStages;
		}

		for( std::size_t i = 0; i < m_currentTextureStages; ++i )
		{
			//const mt::mat4f* uvMask = NULL;

			TextureStage & stage = m_currentTextureStage[i];

			TextureStage & pass_stage = _pass->textureStage[i];

			bool changeMask = false;
			if( _renderObject->textures[i] != NULL )
			{
				const mt::mat4f* uvMask = _renderObject->textures[i]->getUVMask();
				//if( m_uvMask[i] != uvMask )
				{
					m_uvMask[i] = uvMask;
					changeMask = true;
				}
			}

			if( ( _renderObject->textures[i] != NULL 
				&& _renderObject->textures[i]->getID() != m_currentTexturesID[i] )
				|| m_currentTexturesID[i] != 0 )
			{
				m_currentTexturesID[i] = 0;
				RenderImageInterface* t = m_nullTexture->getInterface();
				if( _renderObject->textures[i] != NULL )
				{
					t = _renderObject->textures[i]->getInterface();
					m_currentTexturesID[i] = _renderObject->textures[i]->getID();
				}
				m_interface->setTexture( i, t );
			}

			if( stage.addressU != pass_stage.addressU
				|| stage.addressV != pass_stage.addressV )
			{
				stage.addressU = pass_stage.addressU;
				stage.addressV = pass_stage.addressV;

				m_interface->setTextureAddressing( i
					, stage.addressU
					, stage.addressV );
			}

			if( stage.colorOp != pass_stage.colorOp
				|| stage.colorArg1 != pass_stage.colorArg1
				|| stage.colorArg2 != pass_stage.colorArg2 )
			{
				stage.colorOp = pass_stage.colorOp;
				stage.colorArg1 = pass_stage.colorArg1;
				stage.colorArg2 = pass_stage.colorArg2;

				m_interface->setTextureStageColorOp( i
					, stage.colorOp
					, stage.colorArg1
					, stage.colorArg2 );
			}

			if( stage.alphaOp != pass_stage.alphaOp
				|| stage.alphaArg1 != pass_stage.alphaArg1
				|| stage.alphaArg2 != pass_stage.alphaArg2 )
			{
				stage.alphaOp = pass_stage.alphaOp;
				stage.alphaArg1 = pass_stage.alphaArg1;
				stage.alphaArg2 = pass_stage.alphaArg2;

				m_interface->setTextureStageAlphaOp( i
					, stage.alphaOp
					, stage.alphaArg1
					, stage.alphaArg2 );
			}

			bool changeTexMatrix = false;

			if( stage.matrix != pass_stage.matrix )
			{
				stage.matrix = pass_stage.matrix;
				changeTexMatrix = true;
			}

			if( ( changeMask || changeTexMatrix ) == true )
			{
				const float* textureMatrixBuff = NULL;
				mt::mat4f textureMatrix;
				if( m_uvMask[i] != NULL && stage.matrix != NULL )
				{
					mt::mul_m4_m4( textureMatrix, *m_uvMask[i], *(stage.matrix) );
					textureMatrixBuff = textureMatrix.buff();
				}
				else if( m_uvMask[i] != NULL )
				{
					textureMatrixBuff = m_uvMask[i]->buff();
				}
				else if( stage.matrix != NULL )
				{
					textureMatrixBuff = stage.matrix->buff();
				}
				m_interface->setTextureMatrix( i, textureMatrixBuff );
			}
			
		}

		if( m_currentBlendSrc != _pass->blendSrc )
		{
			m_currentBlendSrc = _pass->blendSrc;
			m_interface->setSrcBlendFactor( m_currentBlendSrc );
		}

		if( m_currentBlendDst != _pass->blendDst )
		{
			m_currentBlendDst = _pass->blendDst;
			m_interface->setDstBlendFactor( m_currentBlendDst );
		}

		//std::size_t primCount = s_getPrimitiveCount( _renderObject->primitiveType, _renderObject->dipIndiciesNum );
		m_interface->drawIndexedPrimitive( _renderObject->primitiveType, 0, _renderObject->minIndex,
			_renderObject->dipVerticesNum, _renderObject->startIndex, _renderObject->dipIndiciesNum );

		++m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableTextureStage_( std::size_t _stage, bool _enable )
	{
		if( _enable == false )
		{
			TextureStage & stage = m_currentTextureStage[_stage];

			m_currentTexturesID[_stage] = 0;

			stage.colorOp = TOP_DISABLE;
			stage.colorArg1 = TARG_TEXTURE;
			stage.colorArg2 = TARG_DIFFUSE;

			m_interface->setTexture( _stage, NULL );
			
			m_interface->setTextureStageColorOp( _stage
				, stage.colorOp
				, stage.colorArg1
				, stage.colorArg2
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::orthoOffCenterLHMatrix_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf )
	{
		mt::ident_m4( _out );

		float inv_lr = 1.0f / ( l - r );
		float inv_bt = 1.0f / ( b - t );
		float inv_znzf = 1.0f / ( zn - zf );
		_out[0][0] = -2.0f * inv_lr;
		_out[1][1] = -2.0f * inv_bt;
		_out[2][2] = -1.0f * inv_znzf;
		_out[3][0] = ( l + r ) * inv_lr;
		_out[3][1] = ( t + b ) * inv_bt;
		_out[3][2] = zn * inv_znzf;
		_out[3][3] = 1.0f;

	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::restoreRenderSystemStates_()
	{
		//mt::mat4f ident;
		
		mt::ident_m4( m_worldTransform );
		mt::ident_m4( m_viewTransform );
		mt::ident_m4( m_projTransform );

		m_interface->setVertexBuffer( m_currentVBHandle );
		m_interface->setIndexBuffer( m_currentIBHandle );
		m_interface->setVertexDeclaration( Vertex2D::declaration );
		m_interface->setProjectionMatrix( m_projTransform.buff() );
		m_interface->setModelViewMatrix( m_viewTransform.buff() );
		//m_interface->setWorldMatrix( m_worldTransform.buff() );
		m_interface->setCullMode( CM_CULL_NONE );
		m_interface->setFillMode( FM_SOLID );
		m_interface->setDepthBufferTestEnable( true );
		m_interface->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
		m_interface->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
		m_interface->setAlphaTestEnable( m_alphaTestEnable );
		m_interface->setAlphaBlendEnable( m_alphaBlendEnable );
		m_interface->setAlphaCmpFunc( CMPF_GREATER_EQUAL, 0x01 );
		m_interface->setLightingEnable( false );
		
		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			TextureStage & stage = m_currentTextureStage[i];

			//stage = TextureStage();

			m_interface->setTextureAddressing( i, stage.addressU, stage.addressV );

			m_interface->setTextureStageColorOp( i
				, stage.colorOp
				, stage.colorArg1
				, stage.colorArg2 
				);

			m_interface->setTextureStageAlphaOp( i
				, stage.alphaOp
				, stage.alphaArg1
				, stage.alphaArg2 );

			ETextureFilter tFilter = m_textureFiltering ? TF_LINEAR : TF_NONE;
			m_interface->setTextureStageFilter( i, TFT_MIPMAP, TF_NONE );
			m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, tFilter );
			m_interface->setTextureStageFilter( i, TFT_MINIFICATION, tFilter );
			
			// skip texture matrix
			m_uvMask[i] = NULL;
			stage.matrix = NULL;
			m_interface->setTextureMatrix( i, NULL );
		}

		//m_currentBlendSrc = BF_ONE;
		m_interface->setSrcBlendFactor( m_currentBlendSrc );

		//m_currentBlendDst = BF_ZERO;
		m_interface->setDstBlendFactor( m_currentBlendDst );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setProjectionMatrix2D_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf )
	{
		//float offsX = m_interface->getTexelOffsetX();
		//float offsY = m_interface->getTexelOffsetY();
		//mt::mat4f temp1, temp2;
		//mt::ident_m4( temp1 );
		//mt::ident_m4( _out );
		//temp1[1][1] = -1.0f;
		//_out[3][0] = -offsX;
		//_out[3][1] = offsY + m_contentResolution[1];
		//mt::mul_m4_m4( temp2, temp1, _out );
		//orthoOffCenterLHMatrix_( temp1, l, r, b, t, zn, zf );
		m_interface->makeProjection2D( l, r, t, b, zn, zf, &(_out.v0[0]) );
		//mt::mul_m4_m4( _out, temp2, temp1 );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setActiveCamera( Camera* _camera )
	{
		if( ( m_activeCamera 
			&& (m_activeCamera->camera == _camera) )
			|| _camera == NULL )
		{
			return;
		}

		//assert( _camera && "Active camera can't be NULL" );


		TVectorRenderCamera::iterator it_end = m_cameras.end();
		TVectorRenderCamera::iterator it_find = it_end;
		if( m_activeCamera != NULL )
		{
			FindCamera pred(_camera);
			TVectorRenderCamera::iterator it_find = 
				std::find_if( m_cameras.begin(), it_end, pred );
		}

		if( it_find == it_end )
		{
			_camera->setRenderTarget( m_currentRenderTarget );
			RenderCamera* rCamera = m_renderCameraPool.get();
			rCamera->camera = _camera;
			rCamera->blendObjects.clear();
			rCamera->solidObjects.clear();
			m_cameras.push_back( rCamera );
			m_activeCamera = m_cameras.back();
		}
		else
		{
			m_activeCamera = (*it_find);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Camera* RenderEngine::getActiveCamera() const
	{
		if( m_activeCamera != NULL )
		{
			return m_activeCamera->camera;
		}
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::batch_( TVectorRenderObject & _objects, size_t _startVertexPos, bool textureSort )
	{
		if( textureSort == true )
		{
			TextureSortPredicate textureSortPred;
			std::sort( _objects.begin(), _objects.end(), textureSortPred );
		}

		RenderObject* batchedRO = NULL;
		size_t verticesNum = _startVertexPos;

		for( TVectorRenderObject::iterator 
			it = _objects.begin(), 
			it_end = _objects.end();
			it != it_end;
			++it )
		{
			RenderObject* cRO = (*it);
			size_t vertexStride = m_primitiveVertexStride[cRO->logicPrimitiveType];
			size_t align = ( vertexStride - ( verticesNum % vertexStride ) ) % vertexStride;
			verticesNum += align + cRO->verticesNum;
			bool batch = checkForBatch_( batchedRO, cRO );
			if( batch == true )
			{
				batchedRO->dipIndiciesNum += cRO->dipIndiciesNum;
				batchedRO->dipVerticesNum += cRO->verticesNum;
				cRO->dipVerticesNum = 0;
				cRO->dipIndiciesNum = 0;
			}
			else
			{
				batchedRO = cRO;
				cRO->dipVerticesNum = cRO->verticesNum;
			}
		}
		return (verticesNum - _startVertexPos);
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderEngine::DebugInfo& RenderEngine::getDebugInfo() const
	{
		return m_debugInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::hasTexture( const ConstString & _name )
	{
		TTextureMap::iterator it_find = m_textures.find( _name );

		if( it_find == m_textures.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::resetFrameCount()
	{
		m_debugInfo.frameCount = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render_()
	{
		if( m_cameras.empty() == true )	// nothing to render
		{
			return;
		}

		for( TVectorRenderCamera::iterator 
			rit = m_cameras.begin(), 
			rit_end = m_cameras.end();
			rit != rit_end;
			++rit )
		{
			Camera* camera = (*rit)->camera;

			//Viewport currentViewport = m_renderViewport;
			const ConstString& renderTarget = camera->getRenderTarget();
			if( renderTarget != m_currentRenderTarget )
			{
				m_currentRenderTarget = renderTarget;
				if( m_currentRenderTarget == Consts::c_Window )
				{
					m_interface->setRenderTarget( NULL, true );
					m_currentRenderViewport = m_renderViewport;
					m_renderTargetResolution = m_windowResolution;
					//m_interface->setRenderViewport( m_renderViewport );
				}
				else
				{
					TTextureMap::iterator it = m_renderTargets.find( m_currentRenderTarget );

					if(it == m_renderTargets.end())
					{
						MENGE_LOG_ERROR("Warning: no render target!");
					}
					else
					{
						Texture* rt = it->second;

						m_interface->setRenderTarget( rt->getInterface(), true );
						m_currentRenderViewport = Viewport( mt::vec2f(0.f, 0.f), mt::vec2f(rt->getWidth(), rt->getHeight()) );
						m_renderTargetResolution = Resolution( rt->getWidth(), rt->getHeight() );
						//m_interface->setRenderViewport( m_currentRenderViewport );
					}
				}
			}

			m_projTransform = camera->getProjectionMatrix();
			//m_interface->setProjectionMatrix( m_projTransform.buff() );

			const Viewport & renderViewport = camera->getRenderViewport();
			applyRenderViewport( renderViewport );

			m_viewTransform = camera->getViewMatrix();
			m_interface->setModelViewMatrix( m_viewTransform.buff() );

			TVectorRenderObject & solidObjects = (*rit)->solidObjects;

			if( solidObjects.empty() == false )
			{
				// render solid from front to back
				//m_interface->setDepthBufferTestEnable( true );
				if( m_depthBufferWriteEnable != true )
				{
					m_depthBufferWriteEnable = true;
					m_interface->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
				}
				if( m_alphaTestEnable != false )
				{
					m_alphaTestEnable = false;
					m_interface->setAlphaTestEnable( m_alphaTestEnable );
				}
				if( m_alphaBlendEnable != false )
				{
					m_alphaBlendEnable = false;
					m_interface->setAlphaBlendEnable( m_alphaBlendEnable );
				}
			}

			for( TVectorRenderObject::reverse_iterator 
				it = solidObjects.rbegin(), 
				it_end = solidObjects.rend();
				it != it_end; ++it )
			{
				RenderObject* renderObject = (*it);
				if( renderObject->dipIndiciesNum == 0 
					|| renderObject->dipVerticesNum == 0 )
				{
					continue;
				}

				renderPass_( renderObject );
			}

			TVectorRenderObject & blendObjects = (*rit)->blendObjects;
			if( blendObjects.empty() == false )
			{
				// render transperent from back to front
				if( m_depthBufferWriteEnable != false )
				{
					m_depthBufferWriteEnable = false;
					m_interface->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
				}
				if( m_alphaBlendEnable != true )
				{
					m_alphaBlendEnable = true;
					m_interface->setAlphaBlendEnable( m_alphaBlendEnable );
				}
				if( m_alphaTestEnable != true )
				{
					m_alphaTestEnable = true;
					m_interface->setAlphaTestEnable( m_alphaTestEnable );
				}
			}
			for( TVectorRenderObject::iterator 
				it = blendObjects.begin(), 
				it_end = blendObjects.end();
				it != it_end; ++it )
			{
				RenderObject* renderObject = (*it);
				if( renderObject->dipIndiciesNum == 0 
					|| renderObject->dipVerticesNum == 0 )
				{
					continue;
				}

				renderPass_( renderObject );
			}		
			releaseRenderCamera_( *rit );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject2D( Material* _material, Texture** _textures, int _texturesNum,
										Vertex2D* _vertices, size_t _verticesNum,
										ELogicPrimitiveType _type )
	{
		RenderObject* ro = m_renderObjectPool.get();
		ro->material = _material;
		ro->textureStages = _texturesNum;
		for( int i = 0; i < _texturesNum; ++i )
		{
			ro->textures[i] = _textures == NULL ? NULL : _textures[i];
		}
		ro->logicPrimitiveType = _type;
		ro->vertexData = (unsigned char*)_vertices;
		ro->verticesNum = _verticesNum;
		ro->dipIndiciesNum = m_primitiveIndexStride[_type] * _verticesNum / m_primitiveVertexStride[_type];

		ApplyZ applyZ( m_layerZ );
		std::for_each( _vertices, _vertices + _verticesNum, applyZ );
		m_layerZ -= 0.0001f;

		switch( _type )
		{
		case LPT_QUAD:
		case LPT_TRIANGLE:
		case LPT_MESH_40_30:
			ro->primitiveType = PT_TRIANGLELIST;
			break;
		case LPT_LINE:
		case LPT_RECTANGLE:
			ro->primitiveType = PT_LINESTRIP;
			break;
		default:
			ro->primitiveType = PT_POINTLIST;
		}

		bool solid = false;
		if( _texturesNum == 1
			&& ro->textures[0] )
		{
			PixelFormat pf = ro->textures[0]->getHWPixelFormat();
			solid = ( pf == PF_R8G8B8 || pf == PF_X8R8G8B8 );
		}
		else if( _texturesNum == 0
			|| ( _texturesNum == 1 && ro->textures[0] == NULL ) )
		{
			solid = true;
		}

		solid = solid && _material->isSolidColor;

		if( solid )
		{
			m_activeCamera->solidObjects.push_back( ro );
		}
		else
		{
			m_activeCamera->blendObjects.push_back( ro );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeBatches_()
	{
		// clear empty cameras
		TVectorRenderCamera::iterator it_end = m_cameras.end();

		for( TVectorRenderCamera::iterator 
			it = m_cameras.begin();
			it != m_cameras.end();
			)
		{
			if( (*it)->blendObjects.empty() && (*it)->solidObjects.empty() )
			{
				m_renderCameraPool.release( (*it) );
				it = m_cameras.erase( it );
			}
			else
			{
				++it;
			}
		}
		
		m_vbPos = 0;
		size_t vbPos = m_vbPos;
		for( TVectorRenderCamera::iterator 
			it = m_cameras.begin(), 
			it_end = m_cameras.end();
			it != it_end;
			++it )
		{
			vbPos += batch_( (*it)->solidObjects, vbPos, true );
			vbPos += batch_( (*it)->blendObjects, vbPos, false );
		}
		size_t vertexDataSize = vbPos - m_vbPos;

		if( vertexDataSize == 0 )	// nothing to render
		{
			return;
		}

		if( vertexDataSize > m_maxVertices2D - m_vbPos )
		{
			m_vbPos = 0;
			vbPos = m_vbPos;
			for( TVectorRenderCamera::iterator 
				it = m_cameras.begin(), 
				it_end = m_cameras.end();
				it != it_end;
				++it )
			{
				vbPos += batch_( (*it)->solidObjects, vbPos, true );
				vbPos += batch_( (*it)->blendObjects, vbPos, false );
			}
			vertexDataSize = vbPos - m_vbPos;
		}

		if( vertexDataSize > m_maxVertices2D )
		{
			MENGE_LOG_ERROR("Warning: vertex buffer overflow");
			//vertexDataSize = m_maxVertices2D;
			setRenderSystemDefaults_( m_primitiveCount[LPT_QUAD] * 2 );
			recreate2DBuffers_( m_maxIndexCount );
			restoreRenderSystemStates_();
			prepare2D_();
			makeBatches_();
			return;
		}

		uint32 lockFlags = m_vbPos ? LOCK_NOOVERWRITE : LOCK_DISCARD;
		m_vbVertexSize = sizeof( Vertex2D );
		unsigned char* vData = 
			(unsigned char*)m_interface->lockVertexBuffer( m_vbHandle2D,
			m_vbPos * m_vbVertexSize,
			vertexDataSize * m_vbVertexSize, lockFlags );

		if( vData == NULL )
		{
			MENGE_LOG_ERROR("Critical error: failed to lock vertex buffer");
		}

		size_t offset = 0;
		
		for( TVectorRenderCamera::iterator 
			it = m_cameras.begin(), 
			it_end = m_cameras.end();
			it != it_end;
			++it )
		{
			offset = insertRenderObjects_( vData, offset, (*it)->solidObjects );
			offset = insertRenderObjects_( vData, offset, (*it)->blendObjects );
		}

		if( m_interface->unlockVertexBuffer( m_vbHandle2D ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to unlock vertex buffer" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::insertRenderObjects_( unsigned char* _vertexBuffer, size_t _offset, TVectorRenderObject& _renderObjects )
	{
		for( TVectorRenderObject::iterator 
			it = _renderObjects.begin(), 
			it_end = _renderObjects.end();
			it != it_end;
			++it )
		{
			RenderObject* ro = (*it);
			ELogicPrimitiveType type = ro->logicPrimitiveType;

			std::size_t indexStart = m_primitiveIndexStart[type];
			std::size_t indexStride = m_primitiveIndexStride[type];
			std::size_t vertexStride = m_primitiveVertexStride[type];

			size_t align = ( vertexStride - ( m_vbPos % vertexStride ) ) % vertexStride;
			_offset += align;
			m_vbPos += align;
			if( m_vbPos + ro->verticesNum > m_maxVertices2D )
			{
				MENGE_LOG_ERROR("Warning: vertex buffer overflow");
				ro->verticesNum = m_maxVertices2D - m_vbPos;
				//return;
			}

			//ro->startIndex = 0;
			//ro->minIndex = 0;
			//if( ro->dipIndiciesNum != 0
			//	&& ro->dipVerticesNum != 0 )
			{


				ro->startIndex  = indexStart + m_vbPos / vertexStride * indexStride;
				ro->minIndex = (ro->startIndex - indexStart) / indexStride * vertexStride;
			}
			assert( ro->startIndex + ro->dipIndiciesNum <= m_maxIndexCount );

			std::copy( ro->vertexData, ro->vertexData + ro->verticesNum * m_vbVertexSize, _vertexBuffer + _offset * m_vbVertexSize );
			m_vbPos += ro->verticesNum;
			_offset += ro->verticesNum;
			//m_logicPrimitiveCount[type] += 1;
		}
		return _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::flushRender_()
	{
		makeBatches_( /*!m_layer3D*/ );
		render_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare2D_()
	{
		if( m_currentVBHandle != m_vbHandle2D )
		{
			m_currentVBHandle = m_vbHandle2D;
			m_interface->setVertexBuffer( m_currentVBHandle );
		}
		if( m_currentIBHandle != m_ibHandle2D )
		{
			m_currentIBHandle = m_ibHandle2D;
			m_interface->setIndexBuffer( m_currentIBHandle );
		}
		if( m_currentVertexDeclaration != Vertex2D::declaration )
		{
			m_currentVertexDeclaration = Vertex2D::declaration;
			m_interface->setVertexDeclaration( m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare3D_()
	{
		if( m_currentVBHandle != m_vbHandle3D )
		{
			m_currentVBHandle = m_vbHandle3D;
			m_interface->setVertexBuffer( m_currentVBHandle );
		}
		if( m_currentIBHandle != m_ibHandle3D )
		{
			m_currentIBHandle = m_ibHandle3D;
			m_interface->setIndexBuffer( m_currentIBHandle );
		}
		if( m_currentVertexDeclaration != Vertex3D::declaration )
		{
			m_currentVertexDeclaration = Vertex3D::declaration;
			m_interface->setVertexDeclaration( m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderCamera_( RenderCamera* _renderCamera )
	{
		for( TVectorRenderObject::iterator it = _renderCamera->solidObjects.begin(),
			it_end = _renderCamera->solidObjects.end();
			it != it_end;
		++it )
		{
			m_renderObjectPool.release( (*it) );
		}
		for( TVectorRenderObject::iterator it = _renderCamera->blendObjects.begin(),
			it_end = _renderCamera->blendObjects.end();
			it != it_end;
		++it )
		{
			m_renderObjectPool.release( (*it) );
		}
		m_renderCameraPool.release( _renderCamera );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::refillIndexBuffer2D_()
	{
		uint16* ibuffer = m_interface->lockIndexBuffer( m_ibHandle2D );

		// QUADS
		size_t vertexCount = 0, maxVertices = 0;
		for( size_t i = m_primitiveIndexStart[LPT_QUAD];
			i + m_primitiveIndexStride[LPT_QUAD] < m_primitiveIndexStart[LPT_TRIANGLE];
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
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_TRIANGLE];
			i + m_primitiveIndexStride[LPT_TRIANGLE] < m_primitiveIndexStart[LPT_LINE];
			i += 3, vertexCount += 3 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 1 + vertexCount;
			ibuffer[i+2] = 2 + vertexCount;
		}
		maxVertices = std::min( maxVertices, vertexCount );
		// LINES
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_LINE];
			i + m_primitiveIndexStride[LPT_LINE] < m_primitiveIndexStart[LPT_RECTANGLE];
			i += 1, vertexCount += 1 )
		{
			ibuffer[i+0] = 0 + vertexCount;
		}
		maxVertices = std::min( maxVertices, vertexCount );
		// RECTANGLES
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_RECTANGLE];
			i + m_primitiveIndexStride[LPT_RECTANGLE] < m_primitiveIndexStart[LPT_MESH_40_30];
			i += 5, vertexCount += 4 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 1 + vertexCount;
			ibuffer[i+2] = 2 + vertexCount;
			ibuffer[i+3] = 3 + vertexCount;
			ibuffer[i+4] = 0 + vertexCount;
		}
		maxVertices = std::min( maxVertices, vertexCount );

		// MESH_40_30
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_MESH_40_30];
			i + m_primitiveIndexStride[LPT_MESH_40_30] < m_maxIndexCount;
			i += m_primitiveIndexStride[LPT_MESH_40_30], vertexCount += m_primitiveVertexStride[LPT_MESH_40_30] )
		{
			size_t counter = 0;
			for( int k = 0; k < 30 - 1; k++ )
			{
				for( int l = 0; l < 40 - 1; l++ )
				{
					uint16 index = (40 * k)+l;
					ibuffer[i+counter+0] = index + vertexCount; 
					ibuffer[i+counter+1] = index + 40 + vertexCount;
					ibuffer[i+counter+2] = index + 1 + vertexCount;
					ibuffer[i+counter+3] = index + 1 + vertexCount;
					ibuffer[i+counter+4] = index + 40 + vertexCount;
					ibuffer[i+counter+5] = index + 40 + 1 + vertexCount;
					counter += 6;
				}
			}
		}
		maxVertices = std::min( maxVertices, vertexCount );

		if( m_interface->unlockIndexBuffer( m_ibHandle2D ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to unlock index buffer" );
		}
		return maxVertices;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::recreate2DBuffers_( std::size_t _maxIndexCount )
	{
		if( m_ibHandle2D != 0 )
		{
			m_interface->releaseIndexBuffer( m_ibHandle2D );
			if( m_currentIBHandle == m_ibHandle2D )
			{
				m_currentIBHandle = 0;
			}
			m_ibHandle2D = 0;
		}

		m_ibHandle2D = m_interface->createIndexBuffer( _maxIndexCount );
		if( m_ibHandle2D == 0 )
		{
			MENGE_LOG_ERROR( "Critical error: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}

		m_maxVertices2D = refillIndexBuffer2D_();

		if( m_vbHandle2D != 0 )
		{
			m_interface->releaseVertexBuffer( m_vbHandle2D );
			if( m_currentVBHandle == m_vbHandle2D )
			{
				m_currentVBHandle = 0;
			}
			m_vbHandle2D = 0;
		}

		m_vbHandle2D = m_interface->createVertexBuffer( m_maxVertices2D, sizeof( Vertex2D ) );
		if( m_vbHandle2D == 0 )
		{
			MENGE_LOG_ERROR( "Critical error: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderSystemDefaults_( int _maxQuadCount )
	{
		mt::ident_m4( m_projTransform );
		mt::ident_m4( m_worldTransform );
		mt::ident_m4( m_viewTransform );


		m_primitiveCount[LPT_QUAD] = _maxQuadCount;
		m_primitiveIndexStride[LPT_QUAD] = 6;
		m_primitiveVertexStride[LPT_QUAD] = 4;
		m_primitiveIndexStride[LPT_TRIANGLE] = 3;
		m_primitiveVertexStride[LPT_TRIANGLE] = 3;
		m_primitiveIndexStride[LPT_LINE] = 1;
		m_primitiveVertexStride[LPT_LINE] = 1;
		m_primitiveIndexStride[LPT_RECTANGLE] = 5;
		m_primitiveVertexStride[LPT_RECTANGLE] = 4;
		m_primitiveIndexStride[LPT_MESH_40_30] = 6786;
		m_primitiveVertexStride[LPT_MESH_40_30] = 1200;
		for( std::size_t i = 1; i != LPT_PRIMITIVE_COUNT; ++i )
		{
			m_primitiveCount[i] = m_primitiveCount[LPT_QUAD] * m_primitiveVertexStride[LPT_QUAD] / m_primitiveVertexStride[i];
		}

		m_primitiveIndexStart[LPT_QUAD] = 0;
		for( std::size_t i = 0; i != LPT_PRIMITIVE_COUNT - 1; ++i )
		{
			m_primitiveIndexStart[i+1] = m_primitiveIndexStart[i] + m_primitiveCount[i] * m_primitiveIndexStride[i];
		}

		for( std::size_t i = 0; i != LPT_PRIMITIVE_COUNT; ++i )
		{
			m_maxIndexCount += m_primitiveCount[i] * m_primitiveIndexStride[i];
		}

		m_currentBlendSrc = BF_ONE;
		m_currentBlendDst = BF_ZERO;
		m_depthBufferWriteEnable = false;
		m_alphaBlendEnable = false;
		m_alphaTestEnable = false;
		for( size_t i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			m_uvMask[i] = NULL;
			m_currentTexturesID[i] = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableTextureFiltering( bool _enable )
	{
		m_textureFiltering = _enable;
		if( m_interface != NULL )
		{
			for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
			{
				ETextureFilter tFilter = m_textureFiltering ? TF_LINEAR : TF_NONE;
				m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, tFilter );
				m_interface->setTextureStageFilter( i, TFT_MINIFICATION, tFilter );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isTextureFilteringEnabled() const
	{
		return m_textureFiltering;
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
			m_interface->setVSync( m_vsync );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getVSync() const
	{
		return m_vsync;
	}
	//////////////////////////////////////////////////////////////////////////
}
