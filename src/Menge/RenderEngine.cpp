#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include <map>

#	include "Application.h"
#	include "Game.h"

#	include "Player.h"
#	include "Arrow.h"
#	include "Math/box2.h"

#	include "ImageCodec.h"
#	include "Codec.h"
#	include "Profiler.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( RenderSystemInterface * _interface )
		: m_interface( _interface )
		, m_windowCreated( false )
		, m_renderFactor( 1.0f )
		, m_layer3D( false )
	{
		mt::ident_m4( m_viewTransform );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize()
	{
		LogSystemInterface * system = Holder<LogEngine>::hostage()->getInterface();

		bool result = m_interface->initialize( system );
		//m_interface->setEventListener( this );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int RenderEngine::getNumDIP() const
	{
		return m_interface->getNumDIP();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
											int _FSAAType, int _FSAAQuality )
	{
		m_fullscreen = _fullscreen;

		m_windowCreated = m_interface->createRenderWindow( _resolution[0], _resolution[1], _bits, _fullscreen, _winHandle,
															_FSAAType, _FSAAQuality );


		recalcRenderArea_( _resolution );
	
		//setRenderTarget( "defaultCamera" );
		//endScene();
		m_currentRenderTarget = "defaultCamera";
		//m_renderViewport.set( mt::vec2f::zero_v2, m_contentResolution );

		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( RenderImageInterface* _image, const mt::vec4f & _rect )
	{
		m_interface->screenshot( _image, _rect.m );
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
		m_layer3D = false;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLayer2D()
	{
		m_interface->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderText(const Menge::String & _text, const mt::vec2f & _pos, unsigned long _color)
	{
		m_interface->renderText(_text,_pos.m,_color);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer3D()
	{
		m_interface->beginLayer3D();
		m_layer3D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setProjectionMatrix( const mt::mat4f& _projection )
	{
		return m_interface->setProjectionMatrix( _projection.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setViewMatrix( const mt::mat4f& _view )
	{
		mt::mat4f view = _view;
		if( m_currentRenderTarget == "defaultCamera" && m_layer3D == false )
		{
			mt::mul_m4_m4( view, _view, m_viewTransform );
		}
		return m_interface->setViewMatrix( view.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	{
		return m_interface->setWorldMatrix( _world.m );
	}	
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createImage( const String& _name, float _width, float _height )
	{
		RenderImageInterface * image = m_interface->createImage( _name, _width, _height );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createRenderTargetImage( const String& _name, const mt::vec2f & _resolution )
	{
		RenderImageInterface * image = m_interface->createRenderTargetImage( _name, _resolution.x, _resolution.y );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::saveImage( RenderImageInterface* _image, const String& _filename )
	{
		String strExt;
		std::size_t pos = _filename.find_last_of( "." );
		if( pos == std::string::npos ) 
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : extension not specified \"%s\""
				, _filename.c_str() );
			return false;
		}

		while( pos != _filename.length() - 1 )
		{
			strExt += _filename[++pos];
		}

		CodecInterface * pCodec = CodecManager::getCodec( strExt );

		if( pCodec == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : invalid extension \"%s\" codec not found"
				, strExt.c_str() );
			return false;
		}

		ImageCodec::ImageData imgData;
		imgData.format = _image->getPixelFormat();
		imgData.height = _image->getHeight();
		imgData.width = _image->getWidth();
		imgData.depth = 1;
		imgData.num_mipmaps = 0;
		imgData.flags = 0;
		imgData.size = 0;	// we don't need this
		int pitch = 0;
		unsigned char* buffer = _image->lock( &pitch );

		unsigned char* lockBuffer = new unsigned char[ imgData.height * pitch ];

		std::size_t mPitch = Menge::PixelUtil::getNumElemBytes( imgData.format ) * imgData.width;
		for( std::size_t i = 0; i != imgData.height; i++ )
		{
			std::copy( buffer, buffer + mPitch, lockBuffer );
			//memcpy( _dstData, _srcData, width * 4 );
			lockBuffer += mPitch;
			buffer += pitch;
		}
		_image->unlock();

		lockBuffer -= mPitch * imgData.height;

		OutStreamInterface* outFile = Holder<FileEngine>::hostage()->openOutStream( _filename, true );
		if( outFile == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : failed to open file for output \"%s\""
				, _filename.c_str() );
			delete[] lockBuffer;
			return false;
		}

		bool res = pCodec->code( outFile, lockBuffer, static_cast<CodecInterface::CodecData*>( &imgData ) );

		delete[] lockBuffer;
		if( res == false )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : Error while coding image data" );
		}

		Holder<FileEngine>::hostage()->closeOutStream( outFile );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::loadImage( const String& _filename, unsigned int _filter )
	{
		RenderImageInterface * image = m_interface->getImage( _filename );

		if(image == 0)
		{
			String strExt;

			std::size_t pos = _filename.find_last_of( "." );

			while( pos != _filename.length() - 1 )
			{
				strExt += _filename[++pos];
			}

			CodecInterface* codec = CodecManager::getCodec( strExt );

			if( codec == 0 )
			{
				MENGE_LOG_ERROR( "Warning: Image codec for extension \"%s\" was not found"
					, strExt.c_str() );
					
				/*MENGE_LOG_CRITICAL( "Художники пидерасы!!!!! Имадж %s не поддерживаемого формата. Пересохранить в пнг 8 бит на канал!!!11адын",
					_filename.c_str() );*/
				return 0;
			}

			DataStreamInterface* stream = Holder<FileEngine>::hostage()->openFile( _filename );

			if( stream == 0 )
			{
				MENGE_LOG_ERROR( "Can't open image file \"%s\""
					, _filename.c_str() );
				//MENGE_LOG_CRITICAL( "Can't open image file \"%s\"", Utils::WToA(_filename).c_str() );
				return 0;
			}

			ImageCodec::ImageData data;
			TextureDesc	textureDesc;

			bool res = codec->getDataInfo( stream, static_cast<CodecInterface::CodecData*>( &data ) );
			if( res == false )
			{
				MENGE_LOG_ERROR( "Error while decoding image \"%s\". Image not loaded"
					, _filename.c_str() );
				Holder<FileEngine>::hostage()->closeStream( stream );
				return 0;
			}
			if( data.format == PF_UNKNOWN )
			{
				/*MENGE_LOG_CRITICAL( "Художники пидерасы!!!!! Имадж %s не поддерживаемого формата. Пересохранить в пнг 8 бит на канал!!!11адын",
					_filename.c_str() );*/
				Holder<FileEngine>::hostage()->closeStream( stream );
				return 0;
			}

			stream->seek( 0 );

			std::size_t image_width = data.width;
			std::size_t image_height = data.height;
			if( ( data.width & ( data.width - 1 ) ) != 0
				|| ( data.height & ( data.height - 1 ) ) != 0 )
			{
				bool npot = m_interface->supportNPOT();
				if( npot == false )	// we're all gonna die
				{

				}
			}
			textureDesc.buffer = new unsigned char[data.size];

			res = codec->decode( stream, (unsigned char*)textureDesc.buffer, data.flags );
			if( res == false )
			{
				MENGE_LOG_ERROR( "Warning: Error while decoding image \"%s\". Image not loaded"
					, _filename.c_str() );
				Holder<FileEngine>::hostage()->closeStream( stream );
				delete[] textureDesc.buffer;
				textureDesc.buffer = 0;
				return 0;
			}

			textureDesc.width = data.width;
			textureDesc.height = data.height;
			textureDesc.size = data.size;
			textureDesc.pixelFormat = data.format;
			textureDesc.name = _filename.c_str();
			textureDesc.filter = _filter;

			Holder<FileEngine>::hostage()->closeStream( stream );

			image = m_interface->loadImage( _filename, image_width, image_height, textureDesc );

			MemoryTextureProfiler::addMemTexture(_filename,data.size);

			delete[] textureDesc.buffer;
			if( image == 0 )
			{
				MENGE_LOG_ERROR( "Render System failed to load image \"%s\""
					, _filename.c_str() );
				return 0;
			}

		}

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(		
			const mt::vec2f * _vertices,
			const mt::vec4f & _uv,
			unsigned int _color, 
			const RenderImageInterface* _image,
			EBlendFactor _src,
			EBlendFactor _dst)
	{
		m_interface->renderImage(
			_vertices->m,
			_uv.m,
			_color,
			_image,
			_src,
			_dst);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderTriple(		
			const mt::vec2f & _a,
			const mt::vec2f & _b,
			const mt::vec2f & _c,
			const mt::vec2f & _uv0,
			const mt::vec2f & _uv1,
			const mt::vec2f & _uv2,
			unsigned int _color, 
			const RenderImageInterface* _image,
			EBlendFactor _src,
			EBlendFactor _dst)
	{
		m_interface->renderTriple(
			_a.m,
			_b.m,
			_c.m,
			_uv0.m,
			_uv1.m,
			_uv2.m,
			_color,
			_image,
			_src,
			_dst);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderLine(	
		unsigned int _color,
		const mt::vec2f & _begin,
		const mt::vec2f & _end)
	{
		m_interface->renderLine( _color, _begin.m, _end.m );		
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseImage( RenderImageInterface * _image )
	{
		MemoryTextureProfiler::removeMemTexture(_image->getDescription());
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
		}

		recalcRenderArea_( resolution );
	}
	//////////////////////////////////////////////////////////////////////////
	CameraInterface * RenderEngine::createCamera( const String& _name )
	{
		return m_interface->createCamera( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	EntityInterface * RenderEngine::createEntity( const String& _name, const String& _meshName )
	{
		return m_interface->createEntity( _name, _meshName );
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * RenderEngine::createLight( const String& _name )
	{
		return m_interface->createLight( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseCamera( CameraInterface * _camera )
	{
		return m_interface->releaseCamera( _camera );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseEntity( EntityInterface * _entity )
	{
		return m_interface->releaseEntity( _entity );
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
		m_interface->beginScene();
		m_interface->setRenderArea( m_renderArea.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		m_interface->endScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::swapBuffers()
	{
		m_interface->swapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderMesh( const std::vector<TVertex>& _vertexData, 
									const std::vector<uint16>& _indexData,
									TMaterial* _material )
	{
		m_interface->renderMesh( &(_vertexData[0]), _vertexData.size(), 
								&(_indexData[0]), _indexData.size(),
								_material );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderArea( const mt::vec4f& _renderArea )
	{
		mt::vec4f renderArea = _renderArea;
		if( m_currentRenderTarget == "defaultCamera" )
		{
			mt::vec2f size = mt::vec2f( _renderArea.z, _renderArea.w ) - mt::vec2f( _renderArea.x, _renderArea.y );
			if( size == mt::vec2f::zero_v2 )
			{
				renderArea = m_renderArea;
			}
			/*else
			{
				mt::box2f area;
				mt::set_box_from_oriented_extent( area, _renderArea.v2_0, _renderArea.v2_1 - _renderArea.v2_0, m_renderTransform );
				renderArea.v2_0 = area.min;
				renderArea.v2_1 = area.max;
			}*/
		}
		m_interface->setRenderArea( renderArea.m );
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

		m_viewTransform.m[0] = areaWidth / crx;
		m_viewTransform.m[5] = areaHeight / cry;
		m_viewTransform.m[12] = m_renderArea.x;
		m_viewTransform.m[13] = m_renderArea.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const String& _target, bool _clear )
	{
		if( m_currentRenderTarget != _target )
		{
			m_currentRenderTarget = _target;
			m_interface->setRenderTarget( m_currentRenderTarget, _clear );
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
}
