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

#	include "Interface/ImageCodecInterface.h"
#	include "Codec.h"
#	include "Profiler.h"
#	include "Bitwise.h"

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
		ImageEncoderInterface* imageEncoder = EncoderManager<ImageEncoderInterface>::createEncoder( _filename );

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
		
		if( bytesWritten == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : Error while encoding image data" );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::loadImage( const String& _filename, unsigned int _filter )
	{
		RenderImageInterface * image = m_interface->getImage( _filename );

		if( image == NULL )
		{

			ImageDecoderInterface* imageDecoder = DecoderManager<ImageDecoderInterface>::createDecoder( _filename );

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
				DecoderManager<ImageDecoderInterface>::releaseDecoder( imageDecoder );
				return NULL;
			}

			std::size_t image_width = dataInfo->width;
			std::size_t image_height = dataInfo->height;

			image = m_interface->createImage( _filename, image_width, image_height, dataInfo->format );
			if( image == NULL )
			{
				MENGE_LOG_ERROR( "Error: RenderSystem couldn't create image" );
				DecoderManager<ImageDecoderInterface>::releaseDecoder( imageDecoder );
				return NULL;
			}

			int pitch = 0;
			PixelFormat pf = image->getPixelFormat();
			unsigned int decoderOptions = 0;
			if( pf != dataInfo->format )
			{
				decoderOptions |= DF_COUNT_ALPHA;
			}
			unsigned char* textureBuffer = image->lock( &pitch, false );

			decoderOptions |= DF_CUSTOM_PITCH;
			decoderOptions |= ( pitch << 16 );

			unsigned int bufferSize = pitch * image->getHeight();
			imageDecoder->setOptions( decoderOptions );
			unsigned int b = imageDecoder->decode( textureBuffer, bufferSize );
			/*if( b == 0 )
			{
				assert( 0 );
			}*/
			DecoderManager<ImageDecoderInterface>::releaseDecoder( imageDecoder );

			// copy pixels on the edge for better image quality
			/*if( data->width > image_width )
			{
				unsigned char* image_data = textureBuffer;
				unsigned int pitch = data->size / data->height;
				unsigned int pixel_size = pitch / data->width;
				for( int i = 0; i < image_height; i++ )
				{
					std::copy( image_data + (image_width - 1) * pixel_size, 
								image_data + image_width * pixel_size,
								image_data + image_width * pixel_size );
					image_data += pitch;
				}
			}
			if( data->height > image_height )
			{
				unsigned char* image_data = textureBuffer;
				unsigned int pitch = data->size / data->height;
				unsigned int pixel_size = pitch / data->width;
				std::copy( image_data + (image_height - 1) * pitch,
							image_data + image_height * pitch,
							image_data + image_height * pitch );
			}*/

			image->unlock();

			MemoryTextureProfiler::addMemTexture( _filename, dataInfo->size );
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
