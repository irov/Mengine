#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include <map>

#	include "Application.h"
#	include "Game.h"

#	include "Interface/ImageCodecInterface.h"
#	include "Codec.h"

#	include "ResourceTexture.h"
#	include "ResourceImage.h"
#	include "RenderObject.h"

#	include <algorithm>

#	include "Camera.h"

namespace Menge
{
	const std::size_t c_renderObjectsNum = 1000;
	const std::size_t c_vertexCount2D = 50000;
	const std::size_t c_vertexCount3D = 50000;
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( RenderSystemInterface * _interface )
		: m_interface( _interface )
		, m_windowCreated( false )
		, m_renderFactor( 1.0f )
		, m_layer3D( false )
		, m_vbHandle2D( 0 )
		, m_ibHandle2D( 0 )
		, m_batchedObject( NULL )
		, m_currentVBHandle( 0 )
		, m_currentIBHandle( 0 )
		, m_currentColor( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_currentTextureStages( 0 )
		, m_currentCamera( NULL )
	{
		mt::ident_m4( m_worldTransfrom );
		mt::ident_m4( m_viewTransform );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::~RenderEngine()
	{
		for( std::vector<RenderObject*>::iterator it = m_renderObjectsPool.begin(),
			it_end = m_renderObjectsPool.end();
			it != it_end;
			++it )
		{
			releaseRenderObject( (*it) );
		}

		for( std::vector<RenderObject*>::iterator it = m_renderObjectsPoolActive.begin(),
			it_end = m_renderObjectsPoolActive.end();
			it != it_end;
			++it )
		{
			releaseRenderObject( (*it) );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize()
	{
		m_renderObjects.reserve( c_renderObjectsNum );
		m_activeObjects.reserve( c_renderObjectsNum );

		LogSystemInterface * system = Holder<LogEngine>::hostage()->getInterface();

		bool result = m_interface->initialize( system );
		//m_interface->setEventListener( this );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t RenderEngine::getNumDIP() const
	{
		return m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
											int _FSAAType, int _FSAAQuality )
	{
		m_fullscreen = _fullscreen;

		m_windowCreated = m_interface->createRenderWindow( _resolution[0], _resolution[1], _bits, _fullscreen, _winHandle,
															_FSAAType, _FSAAQuality );


		if( m_windowCreated == false )
		{
			return false;
		}

		recalcRenderArea_( _resolution );
	
		m_currentRenderTarget = "Window";

		m_vbHandle2D = m_interface->createVertexBuffer( c_vertexCount2D );
		m_ibHandle2D = m_interface->createIndexBuffer( c_vertexCount2D );

		m_vbHandle3D = m_interface->createVertexBuffer( c_vertexCount3D );
		m_ibHandle3D = m_interface->createIndexBuffer( c_vertexCount3D );

		setProjectionMatrix2D_( m_projTranfsorm2D, 0.0f, m_contentResolution[0], 0.0f, m_contentResolution[1], 0.0f, 1.0f );

		mt::ident_m4( m_renderAreaProj );
		//m_interface->setProjectionMatrix( m_projTranfsorm2D.buff() );

		setRenderSystemDefaults_();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( RenderImageInterface* _image, const mt::vec4f & _rect )
	{
		m_interface->screenshot( _image, _rect.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render()
	{
		m_interface->render();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setContentResolution( const Resolution & _resolution )
	{
		m_contentResolution = _resolution;
		std::size_t res[2];
		m_interface->setContentResolution( res );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer2D()
	{
		m_interface->beginLayer2D();
		if( m_layer3D == true )
		{
			setProjectionMatrix( m_projTranfsorm2D );
			m_layer3D = false;
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
		m_interface->beginLayer3D();
		m_layer3D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setProjectionMatrix( const mt::mat4f& _projection )
	{
		RenderObject* ro = getTempRenderObject_();
		ro->setProjTransform = true;
		ro->projTransform = _projection;
		renderObject( ro );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setViewMatrix( const mt::mat4f& _view )
	{
		RenderObject* ro = getTempRenderObject_();
		ro->setViewTransform = true;
		ro->viewTransform = _view;
		renderObject( ro );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	{
		RenderObject* ro = getTempRenderObject_();
		ro->setWorldTransform = true;
		ro->worldTransform = _world;
		renderObject( ro );
	}	
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createImage( const String& _name, float _width, float _height, PixelFormat _format )
	{
		RenderImageInterface * image = m_interface->createImage( _name, 
			::floorf( _width + 0.5f ),
			::floorf( _height + 0.5f ), _format );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createRenderTargetImage( const String& _name, const mt::vec2f & _resolution )
	{
		RenderImageInterface * image = m_interface->createRenderTargetImage( _name,
			::floorf( _resolution.x + 0.5f ),
			::floorf( _resolution.y + 0.5f ) );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::saveImage( RenderImageInterface* _image, const String& _filename )
	{
		ImageEncoderInterface * imageEncoder = Holder<EncoderManager>::hostage()
			->createEncoderT<ImageEncoderInterface>( _filename, "Image" );

		if( imageEncoder == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : can't create encoder for filename \"%s\""
				, _filename.c_str() );
			return false;
		}

		ImageCodecDataInfo dataInfo;
		dataInfo.format = _image->getPixelFormat();
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
		
		Holder<EncoderManager>::hostage()
			->releaseEncoder( imageEncoder );

		if( bytesWritten == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : Error while encoding image data" );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::loadImage( const String& _filename )
	{
		RenderImageInterface * image = m_interface->getImage( _filename );

		if( image == NULL )
		{

			ImageDecoderInterface* imageDecoder = Holder<DecoderManager>::hostage()
				->createDecoderT<ImageDecoderInterface>( _filename, "Image" );

			if( imageDecoder == 0 )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file \"%s\" was not found"
					, _filename.c_str() );
					
				return NULL;
			}

			const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( imageDecoder->getCodecDataInfo() );
			if( dataInfo->format == PF_UNKNOWN )
			{
				MENGE_LOG_ERROR( "Error: Invalid image format \"%s\"",
					_filename.c_str() );
				
				Holder<DecoderManager>::hostage()
					->releaseDecoder( imageDecoder );

				return NULL;
			}

			image = m_interface->loadImage( _filename, imageDecoder );
			//std::size_t image_width = dataInfo->width;
			//std::size_t image_height = dataInfo->height;

			//image = m_interface->createImage( _filename, image_width, image_height, dataInfo->format );
			//if( image == NULL )
			//{
			//	MENGE_LOG_ERROR( "Error: RenderSystem couldn't create image" );
			//	
			//	Holder<DecoderManager>::hostage()
			//		->releaseDecoder( imageDecoder );

			//	return NULL;
			//}

			//int pitch = 0;
			//PixelFormat pf = image->getPixelFormat();
			//unsigned int decoderOptions = 0;
			//if( pf != dataInfo->format )
			//{
			//	decoderOptions |= DF_COUNT_ALPHA;
			//}
			//unsigned char* textureBuffer = image->lock( &pitch, false );

			//decoderOptions |= DF_CUSTOM_PITCH;
			//decoderOptions |= ( pitch << 16 );

			//unsigned int bufferSize = pitch * image->getHeight();
			//imageDecoder->setOptions( decoderOptions );
			//unsigned int b = imageDecoder->decode( textureBuffer, bufferSize );
			/*if( b == 0 )
			{
				assert( 0 );
			}*/
			
			Holder<DecoderManager>::hostage()
				->releaseDecoder( imageDecoder );

			//image->unlock();

			//MemoryTextureProfiler::addMemTexture( _filename, dataInfo->size );
		}

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseImage( RenderImageInterface * _image )
	{
		if( _image->getDescription().find( "Gorodki.png" ) != String::npos )
		{
			int a;
			a = 10;
		}
		//MemoryTextureProfiler::removeMemTexture(_image->getDescription());
		m_interface->releaseImage( _image );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setFullscreenMode( bool _fullscreen )
	{
		if( m_fullscreen == _fullscreen )
		{
			return;
		}

		m_fullscreen = _fullscreen;

		const Resolution & resolution = ( m_fullscreen == true )
			? Holder<Application>::hostage()->getDesktopResolution() 
			: Holder<Game>::hostage()->getResolution();

		if( m_windowCreated )
		{
			m_interface->setFullscreenMode( resolution[0], resolution[1], m_fullscreen );

			Holder<Application>::hostage()->notifyWindowModeChanged( resolution[0], resolution[1], m_fullscreen );
			setRenderSystemDefaults_();
		}

		recalcRenderArea_( resolution );

	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * RenderEngine::createLight( const String& _name )
	{
		return m_interface->createLight( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseLight( LightInterface * _light )
	{
		return m_interface->releaseLight( _light );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::frameStarted()
	{
		m_interface->render();
	}
	//////////////////////////////////////////////////////////////////////////
	Resolution RenderEngine::getBestDisplayResolution( const Resolution & _resolution, float _aspect )
	{
		const std::vector<int> & resolutionList = m_interface->getResolutionList();

		float rx = float(_resolution[0]);
		float ry = float(_resolution[1]);

		float needWidth = ry * _aspect;

		typedef std::vector<int> TResolutionVector;
		typedef std::map< int, TResolutionVector > TResolutionMap;

		TResolutionMap resMap;

		std::size_t count = resolutionList.size() / 2;

		for( std::size_t i = 0; i < count; i++ )
		{
			if( fabsf( ( static_cast<float>( resolutionList[2*i] ) / resolutionList[2*i+1] ) - _aspect ) < 0.01f )
			{
				resMap[ resolutionList[2*i + 1] ].push_back( resolutionList[2*i] );
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

			for( TResolutionVector::size_type 
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
	bool RenderEngine::getFullscreenMode()
	{
		return m_fullscreen;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowMovedOrResized()
	{
		/*if( m_windowCreated )
		{
			m_interface->setViewportDimensions( m_viewportWidth, m_viewportHeight, m_renderFactor );
			m_interface->onWindowMovedOrResized();
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onDeviceRestored()
	{
		const Resolution & resolution = Holder<Game>::hostage()
			->getResolution();

		Holder<Application>::hostage()
			->notifyWindowModeChanged( resolution[0], resolution[1], m_fullscreen );
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
	void RenderEngine::setTextureFiltering( bool _filter )
	{
		m_interface->setTextureFiltering( _filter );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginScene()
	{
		m_activeObjects.clear();
		m_batchedObject = NULL;

		// return active pool objects to pool
		m_renderObjectsPool.insert( m_renderObjectsPool.end(),
									m_renderObjectsPoolActive.begin(),
									m_renderObjectsPoolActive.end() );
		m_renderObjectsPoolActive.clear();

		m_layerZ = 1.0f;
		m_currentRenderTarget = "Window";
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		// prepare vertex and index buffers
		prepareBuffers_();

		// render scene
		m_currentRenderTarget = "Window";
		m_dipCount = 0;
		m_interface->beginScene();
		m_interface->setRenderArea( m_renderArea.buff() );

		for( std::vector<RenderObject*>::iterator it = m_activeObjects.begin(), it_end = m_activeObjects.end();
			it != it_end; ++it )
		{
			RenderObject* renderObject = (*it);
			if( renderObject->batched == true )
			{
				continue;
			}

			// set render target
			if( renderObject->setRenderTarget == true 
				&& renderObject->renderTargetName != m_currentRenderTarget )
			{
				m_currentRenderTarget = renderObject->renderTargetName;
				m_interface->setRenderTarget( m_currentRenderTarget, true );
				m_interface->setProjectionMatrix( m_projTranfsorm2D.buff() );
				//setRenderSystemDefaults_();
			}

			// set transformations
			if( renderObject->setRenderArea == true )
			{
				//setRenderArea( renderObject->renderArea );
				m_interface->setRenderArea( renderObject->renderArea.buff() );
			}
			bool setWorldView = false;
			if( renderObject->setWorldTransform == true )
			{
				m_worldTransfrom = renderObject->worldTransform;
				setWorldView = true;
			}
			if( renderObject->setViewTransform == true )
			{
				m_viewTransform = renderObject->viewTransform;
				setWorldView = true;
			}
			if( setWorldView == true )
			{
				mt::mat4f worldView = m_worldTransfrom * m_viewTransform;
				m_interface->setWorldMatrix( worldView.buff() );
			}
			if( renderObject->setProjTransform == true )
			{
				m_projTransfrom = renderObject->projTransform;
				m_interface->setProjectionMatrix( m_projTransfrom.buff() );
			}

			// render geometry
			if( renderObject->vertices.empty() == true )
			{
				continue;
			}

			if( m_currentVBHandle != renderObject->vbHandle )
			{
				m_currentVBHandle = renderObject->vbHandle;
				m_currentIBHandle = renderObject->ibHandle;
				m_interface->setVertexBuffer( m_currentVBHandle );
				m_interface->setIndexBuffer( m_currentIBHandle );
			}

			for( std::vector<RenderPass>::iterator it = renderObject->passes.begin(),
				it_end = renderObject->passes.end();
				it != it_end;
				++it )
			{
				renderPass_( &(*it), renderObject->vertexIndex, renderObject->verticesNum );
			}

		}

		m_interface->endScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::swapBuffers()
	{
		m_interface->swapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderArea( const mt::vec4f& _renderArea )
	{
		mt::vec4f renderArea = _renderArea;
		if( m_currentRenderTarget == "Window" )
		{
			mt::vec2f size = mt::vec2f( _renderArea.z, _renderArea.w ) - mt::vec2f( _renderArea.x, _renderArea.y );
			if( size == mt::vec2f::zero_v2 )
			{
				renderArea = m_renderArea;
			}
			else
			{
				float rx = static_cast<float>( m_windowResolution[0] ) / m_contentResolution[0];
				float ry = static_cast<float>( m_windowResolution[1] ) / m_contentResolution[1];
				renderArea.x *= rx;
				renderArea.y *= ry;
				renderArea.z *= rx;
				renderArea.w *= ry;
			}
		}

		mt::mat4f proj;// = m_projTransfrom;
		mt::ident_m4( m_renderAreaProj );
		mt::vec2f size = mt::vec2f( _renderArea.z, _renderArea.w ) - mt::vec2f( _renderArea.x, _renderArea.y );
		if( size != mt::vec2f::zero_v2 )
		{
			m_renderAreaProj.v3.x = -renderArea.x * m_contentResolution[0] / ( renderArea.z - renderArea.x );
			m_renderAreaProj.v3.y = -renderArea.y * m_contentResolution[1] / ( renderArea.w - renderArea.y );
			m_renderAreaProj.v0.x = m_contentResolution[0] / ( renderArea.z - renderArea.x );
			m_renderAreaProj.v1.y = m_contentResolution[1] / ( renderArea.w - renderArea.y );
		}

		mt::mul_m4_m4( proj, m_renderAreaProj, m_projTransfrom );

		RenderObject* ro = getTempRenderObject_();
		ro->setRenderArea = true;
		ro->renderArea = renderArea;
		ro->setProjTransform = true;
		ro->projTransform = proj;
		renderObject( ro );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderFactor( float _factor )
	{
		m_renderFactor = _factor;
		if( m_renderFactor < 0.0f )
		{
			MENGE_LOG( "Warning: RenderFactor Value < 0. Setting to 0" );
			m_renderFactor = 0.0f;
		}
		else if( m_renderFactor > 1.0f )
		{
			MENGE_LOG( "Warning: RenderFactor Value > 1. Setting to 1" );
			m_renderFactor = 1.0f;
		}

		float areaWidth = m_renderArea.z - m_renderArea.x;
		float areaHeight = m_renderArea.w - m_renderArea.y;
		

		float crx = float( m_contentResolution[0] );
		float cry = float( m_contentResolution[1] );

		float wrx = float( m_windowResolution[0] );
		float wry = float( m_windowResolution[1] );

		if( m_rendFactPix > 0.0f )
		{
			areaHeight += m_renderFactor * m_rendFactPix * 2;
			areaWidth = areaHeight * crx / cry;

			if( areaWidth > wrx )
			{
				areaWidth = wrx;
			}

			m_renderArea.x = ( wrx - areaWidth ) * 0.5f;
			m_renderArea.y = ( wry - areaHeight ) * 0.5f;
			m_renderArea.z = m_renderArea.x + areaWidth;
			m_renderArea.w = m_renderArea.y + areaHeight;

			m_renderArea.x = ::floorf( m_renderArea.x + 0.5f );
			m_renderArea.y = ::floorf( m_renderArea.y + 0.5f );
			m_renderArea.z = ::floorf( m_renderArea.z + 0.5f );
			m_renderArea.w = ::floorf( m_renderArea.w + 0.5f );

			areaWidth = m_renderArea.z - m_renderArea.x;
			areaHeight = m_renderArea.w - m_renderArea.y;
		}

		m_viewTransform.v0.x = areaWidth / crx;
		m_viewTransform.v1.y = areaHeight / cry;
		m_viewTransform.v3.x = m_renderArea.x;
		m_viewTransform.v3.y = m_renderArea.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const String& _target, bool _clear )
	{
		if( m_currentRenderTarget != _target )
		{
			m_currentRenderTarget = _target;
			RenderObject* ro = getTempRenderObject_();
			ro->setRenderTarget = true;
			ro->renderTargetName = _target;
			renderObject( ro );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& RenderEngine::getRenderArea() const
	{
		return m_renderArea;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::recalcRenderArea_( const Resolution & _resolution )
	{

		float rx = float( _resolution.getWidth());
		float ry = float( _resolution.getHeight());

		m_renderArea.x = 0.0f;
		m_renderArea.y = 0.0f;
		m_renderArea.z = rx;
		m_renderArea.w = ry;

		float one_div_width = 1.f / rx;
		float one_div_height = 1.f / ry;

		float crx = float( m_contentResolution.getWidth() );
		float cry = float( m_contentResolution.getHeight() );

		float contentAspect = crx / cry;
		float dw = crx * one_div_width;
		float dh = cry * one_div_height;
		float aspect = rx * one_div_height;

		if( dw > 1.0f )
		{
			dw = 1.0f;
			dh = rx / contentAspect * one_div_height;
		}

		if( dh > 1.0f )
		{
			dh = 1.0f;
			dw = ry * contentAspect * one_div_width;
		}

		float areaWidth = dw * rx;
		float areaHeight = dh * ry;

		m_renderArea.x = ( rx - areaWidth ) * 0.5f;
		m_renderArea.y = ( ry - areaHeight ) * 0.5f;
		m_renderArea.z = m_renderArea.x + areaWidth;
		m_renderArea.w = m_renderArea.y + areaHeight;

		m_rendFactPix = m_renderArea.y;

		if( m_renderArea.y > 0.0f )
		{
			m_renderFactor = m_renderArea.x / aspect / m_renderArea.y;
		}
		else
		{
			m_renderFactor = 0.0f;
		}

		m_windowResolution = _resolution;

		setRenderFactor( m_renderFactor );
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
	RenderObject* RenderEngine::createRenderObject()
	{
		RenderObject* ro = new RenderObject;
		m_renderObjects.push_back( ro );
		return ro;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject( RenderObject* _renderObject )
	{
		if( m_layer3D == false )
		{
			for( std::vector<TVertex>::iterator it = _renderObject->vertices.begin(), it_end = _renderObject->vertices.end();
				it != it_end;
				++it )
			{
				it->pos[2] = m_layerZ;
			}
			m_layerZ -= 0.0001f;

			_renderObject->vbHandle = m_vbHandle2D;
			_renderObject->ibHandle = m_ibHandle2D;
		}
		else
		{
			_renderObject->vbHandle = m_vbHandle3D;
			_renderObject->ibHandle = m_ibHandle3D;
		}

		_renderObject->verticesNum = 0;
		_renderObject->batched = false;	// reset batching
		bool batched = checkForBatch_( m_batchedObject, _renderObject );
		if( batched == true )
		{
			_renderObject->batched = true;
			if( _renderObject->passes.empty() == false )
			{
				_renderObject->passes[0].batchedIndicies = _renderObject->passes[0].indicies;
				for( std::vector<uint16>::iterator it = _renderObject->passes[0].batchedIndicies.begin(),
					it_end = _renderObject->passes[0].batchedIndicies.end();
					it != it_end;
				++it )
				{
					(*it) += m_batchedObject->verticesNum;
				}
				m_batchedObject->passes[0].indiciesNum += _renderObject->passes[0].indicies.size();
			}
			m_batchedObject->verticesNum += _renderObject->vertices.size();
		}
		else
		{
			_renderObject->batched = false;
			for( std::vector<RenderPass>::iterator it = _renderObject->passes.begin(),
				it_end = _renderObject->passes.end();
				it != it_end;
			++it )
			{
				it->indiciesNum = it->indicies.size();
			}
			_renderObject->verticesNum = _renderObject->vertices.size();
			m_batchedObject = _renderObject;
		}

		m_activeObjects.push_back( _renderObject );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderObject( RenderObject* _renderObject )
	{
		if( _renderObject == NULL ) return;

		std::vector<RenderObject*>::iterator it_find = std::find( m_renderObjects.begin(), m_renderObjects.end(), _renderObject );
		if( it_find != m_renderObjects.end() )
		{
			m_renderObjects.erase( it_find );
		}

		it_find = std::find( m_activeObjects.begin(), m_activeObjects.end(), _renderObject );
		if( it_find != m_activeObjects.end() )
		{
			m_activeObjects.erase( it_find );
		}

		delete _renderObject;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::checkForBatch_( RenderObject* _prev, RenderObject* _next )
	{
		if( _prev == NULL || _next == NULL 
			|| _prev->passes.size() > 1	)				// can't batch more than 1 pass
		{
			return false;
		}

		if( (_prev->passes.empty() == false) &&
			(_prev->passes[0].primitiveType == PT_LINESTRIP		// this primitives could'n be batched
			|| _prev->passes[0].primitiveType == PT_TRIANGLESTRIP 
			|| _prev->passes[0].primitiveType == PT_TRIANGLEFAN ) )
		{
			return false;
		}

		return _prev->operator==( _next );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( RenderPass* _pass, std::size_t _vertexIndex, std::size_t _verticesNum )
	{
		if( _pass->indiciesNum == 0 )	// nothing to render
		{
			return;
		}

		if( _pass->color != m_currentColor )
		{
			m_currentColor = _pass->color;
			m_interface->setTextureFactor( m_currentColor.getAsARGB() );
		}


		if( m_currentTextureStages > _pass->textureStages )
		{
			for( std::size_t i = _pass->textureStages; i < m_currentTextureStages; ++i )
			{
				enableTextureStage_( i, false );
			}
			m_currentTextureStages = _pass->textureStages;
		}
		else if( m_currentTextureStages < _pass->textureStages )
		{
			m_currentTextureStages = _pass->textureStages;
		}

		for( std::size_t i = 0; i < m_currentTextureStages; ++i )
		{
			if( m_currentTextureStage[i].image != _pass->textureStage[i].image 
				|| m_currentTextureStage[i].image_frame != _pass->textureStage[i].image_frame )
			{
				m_currentTextureStage[i].image = _pass->textureStage[i].image;
				m_currentTextureStage[i].image_frame = _pass->textureStage[i].image_frame;
				RenderImageInterface* t = NULL;
				if( m_currentTextureStage[i].image != NULL )
				{
					t = const_cast<RenderImageInterface*>( 
						m_currentTextureStage[i].image->getImage( m_currentTextureStage[i].image_frame ) );
				}
				m_interface->setTexture( i, t );
			}

			if( m_currentTextureStage[i].addressU != _pass->textureStage[i].addressU
				|| m_currentTextureStage[i].addressV != _pass->textureStage[i].addressV )
			{
				m_currentTextureStage[i].addressU = _pass->textureStage[i].addressU;
				m_currentTextureStage[i].addressV = _pass->textureStage[i].addressV;
				m_interface->setTextureAddressing( i, m_currentTextureStage[i].addressU,
													m_currentTextureStage[i].addressV );
			}
		}

		if( m_currentBlendSrc != _pass->blendSrc
			|| m_currentBlendDst != _pass->blendDst )
		{
			m_currentBlendSrc = _pass->blendSrc;
			m_currentBlendDst = _pass->blendDst;
			m_interface->setBlendFactor( m_currentBlendSrc, m_currentBlendDst );
		}

		m_interface->drawIndexedPrimitive( _pass->primitiveType, 
			_vertexIndex, _pass->startIndex, _verticesNum,
			_pass->indiciesNum );

		++m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableTextureStage_( std::size_t _stage, bool _enable )
	{
		if( _enable == false )
		{
			m_interface->setTexture( _stage, NULL );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepareBuffers_()
	{
		TVertex* vertexBuffer2D = m_interface->lockVertexBuffer( m_vbHandle2D );
		uint16* indexBuffer2D = m_interface->lockIndexBuffer( m_ibHandle2D );
		TVertex* vertexBuffer3D = m_interface->lockVertexBuffer( m_vbHandle3D );
		uint16* indexBuffer3D = m_interface->lockIndexBuffer( m_ibHandle3D );
		TVertex* vertexBuffer = vertexBuffer2D;
		uint16* indexBuffer = indexBuffer2D;
		std::size_t vbPos2D = 0, ibPos2D = 0, vbPos3D = 0, ibPos3D = 0;
		std::size_t* vbPos = &vbPos2D;
		std::size_t* ibPos = &ibPos2D;
		for( std::vector<RenderObject*>::iterator it = m_activeObjects.begin(), it_end = m_activeObjects.end();
			it != it_end; ++it )
		{
			RenderObject* renderObject = (*it);

			if( renderObject->vbHandle == m_vbHandle2D )
			{
				vbPos = &vbPos2D;
				ibPos = &ibPos2D;
				vertexBuffer = vertexBuffer2D;
				indexBuffer = indexBuffer2D;
			}
			else
			{
				vbPos = &vbPos3D;
				ibPos = &ibPos3D;
				vertexBuffer = vertexBuffer3D;
				indexBuffer = indexBuffer3D;
			}

			renderObject->vertexIndex = (*vbPos);

			for( std::vector<RenderPass>::iterator it = renderObject->passes.begin(),
				it_end = renderObject->passes.end();
				it != it_end;
			++it )
			{
				RenderPass& pass = (*it);
				pass.startIndex = (*ibPos);

				if( pass.indicies.empty() == false && renderObject->batched == true )
				{
					std::size_t batchedSize = pass.batchedIndicies.size();
					if( (*ibPos) + batchedSize >= c_vertexCount2D )
					{
						MENGE_LOG_ERROR( "Warning: IndexBuffer overflow" );
						break;
					}
					// copy indicies
					std::copy( pass.batchedIndicies.begin(), pass.batchedIndicies.end(), indexBuffer + (*ibPos) );
					(*ibPos) += batchedSize;
				}
				else if( pass.indicies.empty() == false && renderObject->batched == false ) 
				{
					std::size_t indiciesSize = pass.indicies.size();
					if( (*ibPos) + indiciesSize >= c_vertexCount2D )
					{
						MENGE_LOG_ERROR( "Warning: IndexBuffer overflow" );
						break;
					}
					std::copy( pass.indicies.begin(), pass.indicies.end(), indexBuffer + (*ibPos) );
					(*ibPos) += indiciesSize;
				}

			}
			if( renderObject->vertices.empty() == false )
			{
				// copy vertices
				std::size_t verticiesSize = renderObject->vertices.size();
				if( (*vbPos) + verticiesSize >= c_vertexCount2D )
				{
					MENGE_LOG_ERROR( "Warning: VertexBuffer overflow" );
					break;
				}
				std::copy( renderObject->vertices.begin(), renderObject->vertices.end(), vertexBuffer + (*vbPos) );
				(*vbPos) += verticiesSize;
			}
		}
		m_interface->unlockVertexBuffer( m_vbHandle3D );
		m_interface->unlockIndexBuffer( m_ibHandle3D );
		m_interface->unlockVertexBuffer( m_vbHandle2D );
		m_interface->unlockIndexBuffer( m_ibHandle2D );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderObject* RenderEngine::getTempRenderObject_()
	{
		RenderObject* ro = NULL;
		if( m_renderObjectsPool.empty() == false )
		{
			ro = m_renderObjectsPool.back();
			m_renderObjectsPool.pop_back();

			// clear object
			ro->setProjTransform = false;
			ro->setRenderArea = false;
			ro->setViewTransform = false;
			ro->setWorldTransform = false;
			ro->setRenderTarget = false;
			ro->vertices.clear();
			ro->passes.clear();
			ro->ibHandle = 0;
			ro->vbHandle = 0;
		}
		else
		{
			ro = createRenderObject();
		}

		m_renderObjectsPoolActive.push_back( ro );
		return ro;
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
	void RenderEngine::setRenderSystemDefaults_()
	{
		m_interface->setVertexBuffer( m_currentVBHandle );
		m_interface->setIndexBuffer( m_currentIBHandle );
		m_projTransfrom = m_projTranfsorm2D;
		m_interface->setProjectionMatrix( m_projTranfsorm2D.buff() );
		m_interface->setCullMode( CM_CULL_CW );
		m_interface->setTextureAddressing( 0, TAM_CLAMP, TAM_CLAMP );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setProjectionMatrix2D_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf )
	{
		float offsX = m_interface->getTexelOffsetX();
		float offsY = m_interface->getTexelOffsetY();
		mt::mat4f temp1, temp2;
		mt::ident_m4( temp1 );
		mt::ident_m4( _out );
		temp1[1][1] = -1.0f;
		_out[3][0] = -offsX;
		_out[3][1] = offsY + m_contentResolution[1];
		mt::mul_m4_m4( temp2, temp1, _out );
		orthoOffCenterLHMatrix_( temp1, l, r, b, t, zn, zf );
		mt::mul_m4_m4( _out, temp2, temp1 );
	}
	//////////////////////////////////////////////////////////////////////////
}
