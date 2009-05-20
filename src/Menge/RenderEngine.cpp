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
#	include "Material.h"

#	include <algorithm>

#	include "Camera.h"

#	include "Texture.h"
#	include "PixelFormat.h"
#	include "Vertex.h"

#	include <ctime>

namespace Menge
{
	const std::size_t c_vertexCount2D = 25000;
	const std::size_t c_indiciesCount2D = 50000;
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
		, m_currentTextureStages( 0 )
		, m_activeCamera( NULL )
		, m_nullTexture( NULL )
		, m_fullscreen(false)
		, m_currentVertexDeclaration( 0 )
	{
		mt::ident_m4( m_worldTransfrom );
		mt::ident_m4( m_viewTransform );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::~RenderEngine()
	{
		m_interface->releaseVertexBuffer( m_vbHandle2D );
		m_interface->releaseIndexBuffer( m_vbHandle2D );

		if( m_nullTexture )
		{
			releaseTexture( m_nullTexture );
		}

		for( TVectorRenderObjects::const_iterator
			it = m_renderObjects.begin(),
			it_end = m_renderObjects.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		for( TRenderCameraVector::iterator it = m_renderCameraPool.begin(), it_end = m_renderCameraPool.end();
			it != it_end;
			++it )
		{
			delete (*it);
		}
		m_renderCameraPool.clear();

	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize()
	{
		LogSystemInterface * system = Holder<LogEngine>::hostage()->getInterface();

		bool result = m_interface->initialize( system, this );
		//m_interface->setEventListener( this );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
		bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
		m_fullscreen = _fullscreen;

		m_windowCreated = m_interface->createRenderWindow( _resolution[0], _resolution[1], _bits, _fullscreen, _winHandle,
			_waitForVSync, _FSAAType, _FSAAQuality );


		if( m_windowCreated == false )
		{
			return false;
		}

		m_debugInfo.textureMemory = 0;
		m_debugInfo.frameCount = 0;

		m_nullTexture = createTexture( "NullTexture", 2, 2, PF_R8G8B8 );
		int pitch = 0;
		unsigned char* textureData = m_nullTexture->lock( &pitch, false );
		std::fill( textureData, textureData + pitch * 2, 0xFF );
		m_nullTexture->unlock();

		recalcRenderArea_( _resolution );

		m_currentRenderTarget = "Window";

		m_vbHandle2D = m_interface->createVertexBuffer( c_vertexCount2D, sizeof( Vertex2D ) );
		m_ibHandle2D = m_interface->createIndexBuffer( c_indiciesCount2D );

		for( size_t i = 0; i < LPT_MESH; ++i )
		{
			m_primitiveIndexStart[i] = i * c_indiciesCount2D / LPT_MESH;
		}
		uint16* ibuffer = m_interface->lockIndexBuffer( m_ibHandle2D );

		// QUADS
		m_primitiveIndexStride[LPT_QUAD] = 6;
		m_primitiveVertexStride[LPT_QUAD] = 4;
		size_t vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_QUAD];
			i < m_primitiveIndexStart[LPT_TRIANGLE];
			i += 6, vertexCount += 4 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 3 + vertexCount;
			ibuffer[i+2] = 1 + vertexCount;
			ibuffer[i+3] = 1 + vertexCount;
			ibuffer[i+4] = 3 + vertexCount;
			ibuffer[i+5] = 2 + vertexCount;
		}
		// TRIANGLES
		m_primitiveIndexStride[LPT_TRIANGLE] = 3;
		m_primitiveVertexStride[LPT_TRIANGLE] = 3;
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_TRIANGLE];
			i < m_primitiveIndexStart[LPT_LINE];
			i += 3, vertexCount += 3 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 1 + vertexCount;
			ibuffer[i+2] = 2 + vertexCount;
		}
		// LINES
		m_primitiveIndexStride[LPT_LINE] = 1;
		m_primitiveVertexStride[LPT_LINE] = 1;
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_LINE];
			i < m_primitiveIndexStart[LPT_RECTANGLE];
			i += 1, vertexCount += 1 )
		{
			ibuffer[i+0] = 0 + vertexCount;
		}
		// RECTANGLES
		m_primitiveIndexStride[LPT_RECTANGLE] = 5;
		m_primitiveVertexStride[LPT_RECTANGLE] = 4;
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_RECTANGLE];
			i < c_indiciesCount2D;
			i += 5, vertexCount += 4 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 1 + vertexCount;
			ibuffer[i+2] = 2 + vertexCount;
			ibuffer[i+3] = 3 + vertexCount;
			ibuffer[i+4] = 0 + vertexCount;
		}
		m_interface->unlockIndexBuffer( m_ibHandle2D );
		m_vbPos = 0;

		//m_vbHandle3D = m_interface->createVertexBuffer( c_vertexCount3D, sizeof( Vertex3D ) );
		//m_ibHandle3D = m_interface->createIndexBuffer( c_vertexCount3D );

		mt::ident_m4( m_renderAreaProj );
		mt::ident_m4( m_projTransform );
		//m_interface->setProjectionMatrix( m_projTranfsorm2D.buff() );

		m_currentBlendSrc = BF_ONE;
		m_currentBlendDst = BF_ZERO;
		for( int i = 1; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			m_currentTextureStage[i].colorOp = TOP_DISABLE;
		}

		setRenderSystemDefaults_();
		prepare2D_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( Texture* _image, const mt::vec4f & _rect )
	{
		RenderImageInterface* iInterface = _image->getInterface();
		m_interface->screenshot( iInterface, _rect.buff() );
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
	Texture* RenderEngine::createTexture( const String& _name, size_t _width, size_t _height, PixelFormat _format )
	{
		TTextureMap::iterator it_find = m_textures.find( _name );
		if( it_find != m_textures.end() )
		{
			MENGE_LOG_WARNING( "Warning: (RenderEngine::createImage) Image \"%s\" already exist"
				, _name.c_str() );
			return it_find->second;
		}

		size_t hwWidth = _width;
		size_t hwHeight = _height;
		PixelFormat hwPixelFormat = _format;
		RenderImageInterface* image = m_interface->createImage( hwWidth, hwHeight, hwPixelFormat );

		if( image == NULL )
		{
			MENGE_LOG_ERROR( "Error: (RenderEngine::createImage) RenderSystem couldn't create image \"%s\" %dx%d"
				, _name.c_str(), _width, _height );
			return NULL;
		}
		m_debugInfo.textureMemory += PixelUtil::getMemorySize( _width, _height, 1, _format );

		Texture* texture = new Texture( image, _name, _width, _height, _format, hwWidth, hwHeight, hwPixelFormat );
		m_textures.insert( std::make_pair( _name, texture ) );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* RenderEngine::createRenderTargetTexture( const String& _name, const mt::vec2f & _resolution )
	{
		TTextureMap::iterator it_find = m_renderTargets.find( _name );
		if( it_find != m_renderTargets.end() )
		{
			MENGE_LOG_WARNING( "Warning: (RenderEngine::createRenderTargetImage) RenderTarget \"%s\" already exist"
				, _name.c_str() );
			return it_find->second;
		}

		size_t width = ::floorf( _resolution.x + 0.5f );
		size_t height = ::floorf( _resolution.y + 0.5f );
		size_t hwWidth = width;
		size_t hwHeight = height;
		RenderImageInterface * image = m_interface->createRenderTargetImage( hwWidth, hwHeight );

		if( image == NULL )
		{
			MENGE_LOG_ERROR( "Error: (RenderEngine::createRenderTargetImage) RenderSystem couldn't create RenderTarget \"%s\" %dx%d"
				, _name.c_str(), width, height );
			return NULL;
		}

		m_debugInfo.textureMemory += PixelUtil::getMemorySize( width, height, 1, PF_A8R8G8B8 );

		Texture* texture = new Texture( image, _name, width, height, PF_A8R8G8B8, hwWidth, hwHeight, PF_A8R8G8B8 );
		m_renderTargets.insert( std::make_pair( _name, texture ) );
		m_textures.insert( std::make_pair( _name, texture ) );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::saveImage( Texture* _image, const String& _filename )
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
	Texture* RenderEngine::loadTexture( const String& _filename )
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

			rTexture = createTexture( _filename, dataInfo->width, dataInfo->height, dataInfo->format );
			if( rTexture == NULL )
			{
				Holder<DecoderManager>::hostage()
					->releaseDecoder( imageDecoder );
				return NULL;
			}

			rTexture->loadImageData( imageDecoder );

			Holder<DecoderManager>::hostage()
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
			String name = _texture->getName();
			TTextureMap::iterator it_find = m_textures.find( name );
			assert( ( it_find != m_textures.end() ) && "Can't find texture for release" );
			RenderImageInterface* image = _texture->getInterface();
			m_debugInfo.textureMemory -= PixelUtil::getMemorySize( _texture->getWidth(),
				_texture->getHeight(), 1, _texture->getPixelFormat() );
			m_interface->releaseImage( image );
			delete _texture;
			m_textures.erase( it_find );

			it_find = m_renderTargets.find( name );
			if( it_find != m_renderTargets.end() )
			{
				m_renderTargets.erase( it_find );
			}
			//MemoryTextureProfiler::removeMemTexture(_image->getDescription());
		}
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
		//const Resolution & resolution = Holder<Game>::hostage()
		//	->getResolution();

		//Holder<Application>::hostage()
		//	->notifyWindowModeChanged( resolution[0], resolution[1], m_fullscreen );
		setRenderSystemDefaults_();
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
	void RenderEngine::beginScene()
	{
		for( TRenderCameraVector::iterator it = m_cameras.begin(), it_end = m_cameras.end();
			it != it_end;
			++it )
		{
			releaseRenderCamera_( (*it) );
		}
		m_cameras.clear();
		m_activeCamera = NULL;

		m_layerZ = 1.0f;
		m_currentRenderTarget = "Window";
		m_dipCount = 0;
		m_interface->beginScene();
		m_interface->clearFrameBuffer( FBT_COLOR | FBT_DEPTH );
		m_currentRenderArea = m_renderArea;
		m_interface->setRenderArea( m_currentRenderArea.buff() );
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
	void RenderEngine::setRenderArea( const mt::vec4f& _renderArea )
	{
		mt::vec4f renderArea = _renderArea;
		float rx = (m_currentRenderArea.z - m_currentRenderArea.x) / m_contentResolution[0];
		float ry = (m_currentRenderArea.w - m_currentRenderArea.y) / m_contentResolution[1];
		float dx = 0.0f;
		float dy = 0.0f;
		if( renderArea.x < 0.001f
			&& renderArea.y < 0.001f
			&& renderArea.z < 0.001f
			&& renderArea.w < 0.001f )
		{
			renderArea = m_currentRenderArea;
		}
		else
		{
			dx = renderArea.x * rx;
			dy = renderArea.y * ry;
			renderArea.x = dx + m_currentRenderArea.x;
			renderArea.y = dy + m_currentRenderArea.y;
			renderArea.z = renderArea.z * rx + m_currentRenderArea.x;
			renderArea.w = renderArea.w * ry + m_currentRenderArea.y;
		}

		mt::mat4f proj;
		mt::ident_m4( m_renderAreaProj );

		float sx = (m_currentRenderArea.z - m_currentRenderArea.x) / ( renderArea.z - renderArea.x );
		float sy = (m_currentRenderArea.w - m_currentRenderArea.y) / ( renderArea.w - renderArea.y );
		m_renderAreaProj.v3.x = -dx * sx / rx;
		m_renderAreaProj.v3.y = -dy * sy / ry;
		m_renderAreaProj.v0.x = sx;
		m_renderAreaProj.v1.y = sy;

		mt::mul_m4_m4( proj, m_renderAreaProj, m_projTransform );
		m_interface->setProjectionMatrix( proj.buff() );
		m_interface->setRenderArea( renderArea.buff() );
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

		//m_viewTransform.v0.x = areaWidth / crx;
		//m_viewTransform.v1.y = areaHeight / cry;
		//m_viewTransform.v3.x = m_renderArea.x;
		//m_viewTransform.v3.y = m_renderArea.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const String& _target, bool _clear )
	{
		/*if( m_currentRenderTarget != _target )
		{
		m_currentRenderTarget = _target;
		RenderObject* ro = getTempRenderObject_();
		ro->setRenderTarget = true;
		ro->renderTargetName = _target;
		renderObject( ro );
		}*/
		m_currentRenderTarget = _target;
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
	bool RenderEngine::checkForBatch_( RenderObject* _prev, RenderObject* _next )
	{
		if( _prev == NULL || _next == NULL )
		{
			return false;
		}

		if( _prev->primitiveType == PT_LINESTRIP		// this primitives could'n be batched
		|| _prev->primitiveType == PT_TRIANGLESTRIP 
		|| _prev->primitiveType == PT_TRIANGLEFAN )
		{
			return false;
		}

		return _prev->material->operator==( *_next->material );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( RenderObject* _renderObject )
	{
		Material* _pass = _renderObject->material;

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
			const mt::mat4f* uvMask = NULL;
			if( m_currentTextureStage[i].texture != _pass->textureStage[i].texture )
			{
				m_currentTextureStage[i].texture = _pass->textureStage[i].texture;
				RenderImageInterface* t = m_nullTexture->getInterface();
				if( m_currentTextureStage[i].texture != NULL )
				{
					t = m_currentTextureStage[i].texture->getInterface();
					uvMask = m_currentTextureStage[i].texture->getUVMask();
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

			if( m_currentTextureStage[i].colorOp != _pass->textureStage[i].colorOp
				|| m_currentTextureStage[i].colorArg1 != _pass->textureStage[i].colorArg1
				|| m_currentTextureStage[i].colorArg2 != _pass->textureStage[i].colorArg2 )
			{
				m_currentTextureStage[i].colorOp = _pass->textureStage[i].colorOp;
				m_currentTextureStage[i].colorArg1 = _pass->textureStage[i].colorArg1;
				m_currentTextureStage[i].colorArg2 = _pass->textureStage[i].colorArg2;
				m_interface->setTextureStageColorOp( i, m_currentTextureStage[i].colorOp,
													m_currentTextureStage[i].colorArg1,
													m_currentTextureStage[i].colorArg2 );
			}

			if( m_currentTextureStage[i].alphaOp != _pass->textureStage[i].alphaOp
				|| m_currentTextureStage[i].alphaArg1 != _pass->textureStage[i].alphaArg1
				|| m_currentTextureStage[i].alphaArg2 != _pass->textureStage[i].alphaArg2 )
			{
				m_currentTextureStage[i].alphaOp = _pass->textureStage[i].alphaOp;
				m_currentTextureStage[i].alphaArg1 = _pass->textureStage[i].alphaArg1;
				m_currentTextureStage[i].alphaArg2 = _pass->textureStage[i].alphaArg2;
				m_interface->setTextureStageAlphaOp( i, m_currentTextureStage[i].alphaOp,
					m_currentTextureStage[i].alphaArg1,
					m_currentTextureStage[i].alphaArg2 );
			}

			if( uvMask != NULL )	// we must set texture matrix anyway
			{
				const float* textureMatrixBuff = NULL;
				m_currentTextureStage[i].matrix = _pass->textureStage[i].matrix;
				if( m_currentTextureStage[i].matrix != NULL )
				{
					mt::mat4f textureMatrix;
					mt::mul_m4_m4( textureMatrix, *uvMask, *(m_currentTextureStage[i].matrix) );
					textureMatrixBuff = textureMatrix.buff();
				}
				else
				{
					textureMatrixBuff = uvMask->buff();
				}
				m_interface->setTextureMatrix( i, textureMatrixBuff );
			}
			else if( m_currentTextureStage[i].matrix != _pass->textureStage[i].matrix )
			{
				m_currentTextureStage[i].matrix = _pass->textureStage[i].matrix;
				const float* textureMatrixBuff = NULL;
				if( m_currentTextureStage[i].matrix != NULL )
				{
					textureMatrixBuff = m_currentTextureStage[i].matrix->buff();
				}
				m_interface->setTextureMatrix( i, textureMatrixBuff );
			}
		}

		if( m_currentBlendSrc != _pass->blendSrc
			|| m_currentBlendDst != _pass->blendDst )
		{
			m_currentBlendSrc = _pass->blendSrc;
			m_currentBlendDst = _pass->blendDst;
			m_interface->setBlendFactor( m_currentBlendSrc, m_currentBlendDst );
		}

		m_interface->drawIndexedPrimitive( _renderObject->primitiveType, 
			0, _renderObject->startIndex, _renderObject->dipVerticesNum,
			_renderObject->dipIndiciesNum );

		++m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableTextureStage_( std::size_t _stage, bool _enable )
	{
		if( _enable == false )
		{
			m_interface->setTexture( _stage, NULL );
			m_interface->setTextureStageColorOp( _stage, TOP_DISABLE, TARG_TEXTURE, TARG_DIFFUSE );
			m_currentTextureStage[_stage].texture = NULL;
			m_currentTextureStage[_stage].colorOp = TOP_DISABLE;
			m_currentTextureStage[_stage].colorArg1 = TARG_TEXTURE;
			m_currentTextureStage[_stage].colorArg2 = TARG_DIFFUSE;
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
	void RenderEngine::setRenderSystemDefaults_()
	{
		mt::mat4f ident;
		mt::ident_m4( ident );

		m_interface->setVertexBuffer( m_currentVBHandle );
		m_interface->setIndexBuffer( m_currentIBHandle );
		m_interface->setProjectionMatrix( ident.buff() );
		m_interface->setViewMatrix( ident.buff() );
		m_interface->setWorldMatrix( ident.buff() );
		m_interface->setCullMode( CM_CULL_NONE );
		m_interface->setFillMode( FM_SOLID );
		m_interface->setDepthBufferTestEnable( true );
		m_interface->setDepthBufferWriteEnable( false );
		m_interface->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
		m_interface->setAlphaTestEnable( false );
		m_interface->setAlphaBlendEnable( false );
		m_interface->setAlphaCmpFunc( CMPF_GREATER_EQUAL, 0x01 );
		m_interface->setLightingEnable( false );
		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			m_interface->setTextureAddressing( i, m_currentTextureStage[i].addressU,
													m_currentTextureStage[i].addressV );
			m_interface->setTextureStageColorOp( i, m_currentTextureStage[i].colorOp, 
				m_currentTextureStage[i].colorArg1, m_currentTextureStage[i].colorArg2 );
			m_interface->setTextureStageAlphaOp( i, m_currentTextureStage[i].alphaOp, 
				m_currentTextureStage[i].alphaArg1, m_currentTextureStage[i].alphaArg2 );
			m_interface->setTextureStageFilter( i, TFT_MIPMAP, TF_NONE );
			m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, TF_LINEAR );
			m_interface->setTextureStageFilter( i, TFT_MINIFICATION, TF_LINEAR );
			m_interface->setTextureMatrix( i, NULL );
		}
		m_interface->setBlendFactor( m_currentBlendSrc, m_currentBlendDst );
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
	void RenderEngine::setActiveCamera( Camera* _camera )
	{
		if( ( m_activeCamera 
			&& (m_activeCamera->camera == _camera) )
			|| _camera == NULL )
		{
			return;
		}

		//assert( _camera && "Active camera can't be NULL" );


		TRenderCameraVector::iterator it_end = m_cameras.end();
		TRenderCameraVector::iterator it_find = it_end;
		if( m_activeCamera != NULL )
		{
			FindCamera pred(_camera);
			TRenderCameraVector::iterator it_find = 
				std::find_if( m_cameras.begin(), it_end, pred );
		}

		if( it_find == it_end )
		{
			_camera->setRenderTarget( m_currentRenderTarget );
			RenderCamera* rCamera = getRenderCamera_();
			rCamera->camera = _camera;
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
	size_t RenderEngine::batch_( std::vector<RenderObject*>& _objects, size_t _startVertexPos, bool textureSort )
	{
		if( textureSort == true )
		{
			TextureSortPredicate textureSortPred;
			std::sort( _objects.begin(), _objects.end(), textureSortPred );
		}
		RenderObject* batchedRO = NULL;
		size_t verticesNum = _startVertexPos;
		for( std::vector<RenderObject*>::iterator it = _objects.begin(), it_end = _objects.end();
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
	bool RenderEngine::hasTexture( const String& _name )
	{
		TTextureMap::iterator it_find = m_textures.find( _name );
		if( it_find != m_textures.end() )
		{
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderCamera* RenderEngine::getRenderCamera_()
	{
		if( m_renderCameraPool.empty() == false )
		{
			RenderCamera*& rc = m_renderCameraPool.back();
			m_renderCameraPool.pop_back();
			return rc;
		}
		// else
		return new RenderCamera();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderCamera_( RenderCamera* _renderCamera )
	{
		_renderCamera->camera = NULL;
		_renderCamera->solidObjects.clear();
		_renderCamera->blendObjects.clear();
		m_renderCameraPool.push_back( _renderCamera );
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

		for( TRenderCameraVector::iterator rit = m_cameras.begin(), rit_end = m_cameras.end();
			rit != rit_end;
			++rit )
		{
			Camera* camera = (*rit)->camera;

			const String& renderTarget = camera->getRenderTarget();
			if( renderTarget != m_currentRenderTarget )
			{
				m_currentRenderTarget = renderTarget;
				if( m_currentRenderTarget == "Window" )
				{
					m_interface->setRenderTarget( NULL, true );
					m_currentRenderArea = m_renderArea;
					m_interface->setRenderArea( m_currentRenderArea.buff() );
				}
				else
				{
					Texture* rt = m_renderTargets[ m_currentRenderTarget ];
					m_interface->setRenderTarget( rt->getInterface(), true );
					m_currentRenderArea = mt::vec4f( 0.0f, 0.0f, rt->getWidth(), rt->getHeight() );
					m_interface->setRenderArea( m_currentRenderArea.buff() );
				}
			}

			m_projTransform = camera->getProjectionMatrix();
			//m_interface->setProjectionMatrix( m_projTransform.buff() );

			const mt::vec4f& renderArea = camera->getRenderArea();
			setRenderArea( renderArea );

			m_viewTransform = camera->getViewMatrix();
			m_interface->setViewMatrix( m_viewTransform.buff() );

			std::vector<RenderObject*>& solidObjects = (*rit)->solidObjects;
			if( solidObjects.empty() == false )
			{
				// render solid from front to back
				//m_interface->setDepthBufferTestEnable( true );
				m_interface->setDepthBufferWriteEnable( true );
				m_interface->setAlphaTestEnable( false );
				m_interface->setAlphaBlendEnable( false );
			}
			for( std::vector<RenderObject*>::reverse_iterator it = solidObjects.rbegin(), it_end = solidObjects.rend();
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

			std::vector<RenderObject*>& blendObjects = (*rit)->blendObjects;
			if( blendObjects.empty() == false )
			{
				// render transperent from back to front
				m_interface->setDepthBufferWriteEnable( false );
				m_interface->setAlphaBlendEnable( true );
				m_interface->setAlphaTestEnable( true );
			}
			for( std::vector<RenderObject*>::iterator it = blendObjects.begin(), it_end = blendObjects.end();
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

		}
	}
	//////////////////////////////////////////////////////////////////////////
	Material* RenderEngine::createMaterial()
	{
		return new Material();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseMaterial( Material* _material )
	{
		if( _material == NULL )
		{
			return;
		}
		delete _material;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject2D( Material* _material, Vertex2D* _vertices, size_t _verticesNum, ELogicPrimitiveType _type )
	{
		RenderObject* ro = getRenderObject_();
		ro->material = _material;
		ro->logicPrimitiveType = _type;
		ro->vertexData = (unsigned char*)_vertices;
		ro->verticesNum = _verticesNum;
		ro->dipIndiciesNum = m_primitiveIndexStride[_type] * _verticesNum / m_primitiveVertexStride[_type];

		ApplyZ applyZ( m_layerZ );
		std::for_each( _vertices, _vertices + _verticesNum, applyZ );
		m_layerZ -= 0.001f;

		switch( _type )
		{
		case LPT_QUAD:
		case LPT_TRIANGLE:
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
		if( _material->textureStages == 1
			&& _material->textureStage[0].texture )
		{
			PixelFormat pf = _material->textureStage[0].texture->getHWPixelFormat();
			solid = ( pf == PF_R8G8B8 || pf == PF_X8R8G8B8 );
		}
		else if( _material->textureStage == 0
			|| ( _material->textureStages == 1 
			&& _material->textureStage[0].texture == NULL ) )
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
	RenderObject* RenderEngine::getRenderObject_()
	{
		return new RenderObject();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderObject( RenderObject* _renderObject )
	{
		delete _renderObject;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeBatches_()
	{
		// clear empty cameras
		TRenderCameraVector::iterator it_end = m_cameras.end();
		TRenderCameraVector::iterator it_remove = std::stable_partition( m_cameras.begin(), m_cameras.end(), NotEmptyCamera() );
		for( TRenderCameraVector::iterator it = it_remove;
			it != it_end;
			++it )
		{
			releaseRenderCamera_( (*it) );
		}
		m_cameras.erase( it_remove, it_end );
		

		size_t vbPos = m_vbPos;
		for( TRenderCameraVector::iterator it = m_cameras.begin(), it_end = m_cameras.end();
			it != it_end;
			++it )
		{
			vbPos += batch_( (*it)->solidObjects, vbPos, true );
			vbPos += batch_( (*it)->blendObjects, vbPos, false );
		}
		size_t vertexDataSize = vbPos - m_vbPos;

		if( vertexDataSize > c_vertexCount2D - m_vbPos )
		{
			m_vbPos = 0;
			vbPos = m_vbPos;
			for( TRenderCameraVector::iterator it = m_cameras.begin(), it_end = m_cameras.end();
				it != it_end;
				++it )
			{
				vbPos += batch_( (*it)->solidObjects, vbPos, true );
				vbPos += batch_( (*it)->blendObjects, vbPos, false );
			}
			vertexDataSize = vbPos - m_vbPos;
		}

		if( vertexDataSize > c_vertexCount2D )
		{
			MENGE_LOG_ERROR("Warning: vertex buffer overflow");
			vertexDataSize = c_vertexCount2D;
		}

		m_vbVertexSize = sizeof( Vertex2D );
		unsigned char* vData = (unsigned char*)m_interface->lockVertexBuffer( m_vbHandle2D, m_vbPos * m_vbVertexSize, vertexDataSize * m_vbVertexSize );
		size_t offset = 0;

		for( TRenderCameraVector::iterator it = m_cameras.begin(), it_end = m_cameras.end();
			it != it_end;
			++it )
		{
			offset = insertRenderObjects_( vData, offset, (*it)->solidObjects );
			offset = insertRenderObjects_( vData, offset, (*it)->blendObjects );
		}

		m_interface->unlockVertexBuffer( m_vbHandle2D );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::insertRenderObjects_( unsigned char* _vertexBuffer, size_t _offset, TRenderObjectVector& _renderObjects )
	{
		for( TRenderObjectVector::iterator it = _renderObjects.begin(), it_end = _renderObjects.end();
			it != it_end;
			++it )
		{
			RenderObject* ro = (*it);
			ELogicPrimitiveType type = ro->logicPrimitiveType;

			size_t vertexStride = m_primitiveVertexStride[type];
			size_t align = ( vertexStride - ( m_vbPos % vertexStride ) ) % vertexStride;
			_offset += align;
			m_vbPos += align;
			if( m_vbPos + ro->verticesNum > c_vertexCount2D )
			{
				MENGE_LOG_ERROR("Warning: vertex buffer overflow");
				ro->verticesNum = c_vertexCount2D - m_vbPos;
				//return;
			}
			ro->startIndex  = m_primitiveIndexStart[type] + m_vbPos / m_primitiveVertexStride[type] * m_primitiveIndexStride[type];
			if( ro->startIndex + ro->dipIndiciesNum > c_indiciesCount2D )
			{
				assert( 0 );
			}

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
}
