#	include "Video.h"

#	include "ResourceManager.h"

#	include "ResourceVideo.h"
//#	include "ResourceSound.h"
#	include "SoundEmitter.h"
#	include "NodeManager.h"

#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "LogEngine.h"
#	include "pybind/system.hpp"
#	include "Consts.h"

#	include "Interface/VideoCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Video::Video()
		: m_resourceVideo( NULL )
		, m_soundEmitter( NULL )
		, m_autoStart(false)
		, m_needUpdate( false )
		, m_material( NULL )
		, m_frameSize(0.0f, 0.0f)
		, m_videoDecoder(NULL)
		, m_videoFile(NULL)
		, m_timing(0.0f)
	{
		m_textures[0] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		Eventable::registerEvent( EVENT_VIDEO_END, ("onVideoEnd"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setVideoResource( const ConstString& _resource )
	{
		if( m_resourceVideoName == _resource )
		{
			return;
		}

		m_resourceVideoName = _resource;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Video::getVideoResource() const
	{
		return m_resourceVideoName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setSoundResource( const ConstString& _resource )
	{
		if( m_resourceSoundName == _resource )
		{
			return;
		}

		m_resourceSoundName = _resource;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Video::getSoundResource() const
	{
		return m_resourceSoundName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_update( float _timing )
	{
		float speedFactor = this->getSpeedFactor();
		
		float timing = speedFactor * _timing;
		
		Node::_update( timing );
		//localHide(false);
		//printf("%f %s\n",_timing,m_name.c_str());
		if( isPlay() == false )
		{
			return;
		}

		//printf("%f %s\n",_timing,m_name.c_str());
		m_needUpdate = this->_sync( timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}
		//printf("%s  onActivate \n",m_name.c_str());	
		while ( true )
		{
			EVideoDecoderReadState  state  = m_videoDecoder->readNextFrame();
			if( state == VDRS_SKIP )
			{
				continue;
			}
		
			break;
		}

		this->_fillVideoBuffer();
		//m_videoDecoder->seek(0.0f);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_deactivate()
	{
		Node::_deactivate();
		this->_rewind();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_compile()
	{
		m_resourceVideo = ResourceManager::get()
			->getResourceT<ResourceVideo>( m_resourceVideoName );

		if( m_resourceVideo == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Video Resource not found '%s'"
				, m_resourceVideoName.c_str() 
				);	

			return false;
		}

		m_materialGroup = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(BlendSprite) );
				
		Menge::PixelFormat colorMode;

		if ( this->_compileDecoder() == false )
		{
			MENGE_LOG_ERROR( "Warning: Video can`t create video decoder '%s'"
				, m_resourceVideoName.c_str() 
				);	

			return false;
		}

		if ( m_resourceVideo->isAlpha() == 1 )
		{
			colorMode = Menge::PF_A8R8G8B8;
		}
		else
		{
			colorMode = Menge::PF_R8G8B8;
		}

		m_textures[0] = RenderEngine::get()
			->createTexture( m_frameSize.x, m_frameSize.y, colorMode );

		//m_material->textureStage[0].texture = m_resourceImage;

		if( m_resourceSoundName.empty() == false )
		{
			ConstString c_VideoSound("VideoSound");
			m_soundEmitter = NodeManager::get()
				->createNodeT<SoundEmitter>( c_VideoSound, Consts::get()->c_SoundEmitter, Consts::get()->c_builtin_empty );

			addChildren( m_soundEmitter );
			m_soundEmitter->setSoundResource( m_resourceSoundName );
			if( m_soundEmitter->compile() == false )
			{
				MENGE_LOG_ERROR( "Warning: video failed to compile sound resource '%s'"
					, m_resourceSoundName.c_str() 
					);

				return false;
			}
		}

		invalidateVertices();
		invalidateBoundingBox();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_compileDecoder()
	{
		const ConstString & category = m_resourceVideo->getCategory();

		const WString filePath = m_resourceVideo->getFilePath();
		
		m_videoFile = FileEngine::get()
			->openInputFile( category, filePath );

		if( m_videoFile == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't open video file '%S'"
				, filePath.c_str()
				);

			return false;
		}

		const ConstString & codecType = m_resourceVideo->getCodecType();

		m_videoDecoder = CodecEngine::get()
			->createDecoderT<VideoDecoderInterface>( codecType, m_videoFile );

		if( m_videoDecoder == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't create video decoder for file '%S'"
				, filePath.c_str()
				);

			m_videoFile->close();
			return false;
		}
		
		bool alpha = m_resourceVideo->isAlpha();
		VideoCodecOptions videoCodecOptions;
		
		if( alpha == true )
		{
			videoCodecOptions.pixelFormat = Menge::PF_A8R8G8B8;
		}
		else
		{
			videoCodecOptions.pixelFormat = Menge::PF_R8G8B8;
		}
		
		CodecOptions* codecOptions = dynamic_cast<CodecOptions*>(&videoCodecOptions);

		m_videoDecoder->setOptions(  codecOptions );

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();
		m_frameSize.x = dataInfo->frameWidth;
		m_frameSize.y = dataInfo->frameHeight;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_release()
	{
		RenderEngine::get()
			->releaseTexture( m_textures[0] );
		
		if( m_videoDecoder != NULL )
		{
			m_videoDecoder->destroy();
			m_videoDecoder = NULL;
		}

		if( m_videoFile != NULL )
		{
			m_videoFile->close();
			m_videoFile = NULL;
		}
		
		if( m_resourceVideo != 0 )
		{
			m_resourceVideo->decrementReference();
			m_resourceVideo = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_stop( size_t _enumerator )
	{
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->stop();
		}

		m_needUpdate = false;
		this->_rewind( );
		//_release();
		//_compile();

		this->callEventDeferred( EVENT_VIDEO_END, "(OiO)", this->getEmbed() ,_enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_end( size_t _enumerator )
	{
		this->_rewind( );
		
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->stop();
		}

		this->callEventDeferred( EVENT_VIDEO_END, "(OiO)", this->getEmbed() ,_enumerator, pybind::get_bool(false));	
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::pause()
	{
		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_play()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Video: '%s' play not activate"
				, getName().c_str()
				);
			return false;
		}

		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->play();
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_restart( size_t _enumerator )
	{
		if( isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );

		if( m_needUpdate )
		{
			this->_fillVideoBuffer();
			m_needUpdate = false;
		}

		const Vertex2D * vertices = this->getVertices();

		m_material = m_materialGroup->getMaterial( TAM_CLAMP, TAM_CLAMP );

		RenderEngine::get()
			->renderObject2D( _camera, m_material, m_textures, NULL, 1, vertices, 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices )
	{
		const mt::mat4f & wm = this->getWorldMatrix();

		//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
		//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
		//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
		//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
		
		mt::vec3f transformX;
		mt::vec3f transformY;

		mt::mul_v3_m4_r( transformX, mt::vec3f( m_frameSize.x, 0.f, 0.f ), wm );
		mt::mul_v3_m4_r( transformY, mt::vec3f( 0.f, m_frameSize.y, 0.f ), wm );

		mt::vec3f vertices[4];

		vertices[0] = wm.v3.to_vec3f();
		vertices[1] = vertices[0] + transformX;
		vertices[2] = vertices[1] + transformY;
		vertices[3] = vertices[0] + transformY;

		for( int i = 0; i < 4; i++ )
		{
			_vertices[i].pos[0] = vertices[i].x;
			_vertices[i].pos[1] = vertices[i].y;
			_vertices[i].pos[2] = vertices[i].z;
			//_vertices[i].pos[3] = 1.f;
		}

		_vertices[0].uv[0] = 0.0f;
		_vertices[0].uv[1] = 0.0f;
		_vertices[1].uv[0] = 1.0f;
		_vertices[1].uv[1] = 0.0f;
		_vertices[2].uv[0] = 1.0f;
		_vertices[2].uv[1] = 1.0f;
		_vertices[3].uv[0] = 0.0f;
		_vertices[3].uv[1] = 1.0f;

		ColourValue color;
		this->calcTotalColor(color);

		uint32 argb = color.getAsARGB();
		ApplyColor2D applyColor( argb );
		std::for_each( _vertices, _vertices + 4, applyColor );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		const Vertex2D * vertcies = this->getVertices();

		mt::vec2f v( vertcies[0].pos[0], vertcies[0].pos[1] );
		mt::reset( _boundingBox, v );

		for( int i = 1; i != 4; ++i )
		{
			v.x = vertcies[i].pos[0];
			v.y = vertcies[i].pos[1];
			mt::add_internal_point( _boundingBox, v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateColor()
	{
		Node::_invalidateColor();

		invalidateVertices();
	}
	////////////////////////////////////////////////////////////////////
	bool Video::_sync( float _timing )
	{
		m_timing += _timing;
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();
		
		int countFrames = int(m_timing / dataInfo->frameTiming);
		bool needUpdate = false; 
		int frame = countFrames;

		if( frame != -1 )
		{
			needUpdate = true;
		}
				
		while( countFrames > 0 )
		{
			EVideoDecoderReadState state = m_videoDecoder->readNextFrame();

			if( state == VDRS_END_STREAM )
			{	
				this->onEndStream_();

				break;	
			}
			else if( state == VDRS_FAILURE )
			{
				MENGE_LOG_ERROR( "Video: '%s' error reading frame timing %4.2f total timing %4.2f"
					, getName().c_str()
					, _timing
					, m_timing
					);

				break;	
			}
			else if( state == VDRS_SKIP )
			{
				continue;	
			}

			countFrames--;
		}

		m_timing -= frame *  dataInfo->frameTiming;	
		return needUpdate;
	}
	////////////////////////////////////////////////////////////////////
	void Video::onEndStream_()
	{
		if( this->getLoop() == true )
		{
			this->_rewind();
		}
		else
		{
			m_needUpdate = false;

			this->stop();
		}
	}
	////////////////////////////////////////////////////////////////////
	void Video::_rewind()
	{
		m_videoDecoder->seek(0.0f);
	}
	////////////////////////////////////////////////////////////////////
	void Video::_setTiming( float _timing )
	{
		m_videoDecoder->seek(_timing);
	}
	////////////////////////////////////////////////////////////////////
	float Video::_getTiming() const
	{
		float timing = m_videoDecoder->getTiming();
		return timing;
	}
	////////////////////////////////////////////////////////////////////
	void Video::_setFirstFrame()
	{
		m_videoDecoder->seek(0.0f);
	}
	////////////////////////////////////////////////////////////////////
	void Video::_setLastFrame()
	{
	}
	////////////////////////////////////////////////////////////////////
	void Video::_fillVideoBuffer()
	{
		int pitch = 0;
		//Texture* renderImage = m_material->textureStage[0].texture;

		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = (size_t)m_frameSize.x;
		rect.bottom = (size_t)m_frameSize.y;

		unsigned char* lockRect = m_textures[0]->lock( &pitch, rect, false );

		m_videoDecoder->decode( lockRect, pitch );
		m_textures[0]->unlock();
	}
	////////////////////////////////////////////////////////////////////
	void Video::_interrupt( size_t _enumerator )
	{
		//empty
	}
	////////////////////////////////////////////////////////////////////
}
