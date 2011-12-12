#	include "Video.h"

#	include "ResourceManager.h"

#	include "ResourceVideo.h"
//#	include "ResourceSound.h"
#	include "SoundEmitter.h"
#	include "NodeManager.h"

#	include "BinParser.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "LogEngine.h"
#	include "pybind/system.hpp"
#	include "Texture.h"
#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Video::Video()
		: m_resourceVideo( NULL )
		, m_soundEmitter( NULL )
		, m_autoStart(false)
		, m_needUpdate( false )
		, m_timing( 0.0f )
		, m_material( NULL )
	{
		m_textures[0] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::ResourceVideo_Name, m_resourceVideoName );
			BIN_CASE_ATTRIBUTE( Protocol::ResourceSound_Name, m_resourceSoundName );
			BIN_CASE_ATTRIBUTE( Protocol::Looping_Value, m_loop );
			BIN_CASE_ATTRIBUTE( Protocol::AutoStart_Value, m_autoStart );			
		}
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
		Node::_update( _timing );

		if( m_play == false )
		{
			return; 
		}

		m_timing += _timing;
		m_needUpdate = m_resourceVideo->sync( _timing );
		
		if( m_resourceVideo->eof() == true )
		{
			stop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		if( m_autoStart )
		{
			_play();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_deactivate()
	{
		Node::_deactivate();
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

		m_material = m_materialGroup->getMaterial( TAM_CLAMP, TAM_CLAMP );

		const mt::vec2f & size = m_resourceVideo->getFrameSize();
		
		Menge::PixelFormat colorMode;
		if (m_resourceVideo->isAlpha() == 0 )
		{
			colorMode = Menge::PF_A8R8G8B8;
		}
		else
		{
			colorMode = Menge::PF_R8G8B8;
		}

		m_textures[0] = RenderEngine::get()
			->createTexture( m_resourceVideoName, size.x, size.y, colorMode );

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
	void Video::_release()
	{
		RenderEngine::get()
			->releaseTexture( m_textures[0] );

		ResourceManager::get()
			->releaseResource( m_resourceVideo );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_stop( std::size_t _enumerator )
	{
		m_resourceVideo->rewind( );
		m_timing = 0.0f;
		
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->stop();
		}
		_release();
		_compile();
		this->callEvent( EVENT_VIDEO_END, "(OiO)", this->getEmbed() ,_enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_end( std::size_t _enumerator )
	{
		m_resourceVideo->rewind( );
		m_timing = 0.0f;
		
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->stop();
		}

		this->callEvent( EVENT_VIDEO_END, "(OiO)", this->getEmbed() ,_enumerator, pybind::get_bool(false));	
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

		m_play = true;
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->play();
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_restart( std::size_t _enumerator )
	{
		if( isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( Camera2D * _camera )
	{
		if( m_play == false)
		{
			return;
		}

		Node::_render( _camera );
		if( m_needUpdate )
		{
			int pitch = 0;
			//Texture* renderImage = m_material->textureStage[0].texture;
			unsigned char* lockRect = m_textures[0]->lock( &pitch, false );
			m_resourceVideo->getRGBData( lockRect, pitch );
			m_textures[0]->unlock();
			m_needUpdate = false;
		}

		const Vertex2D * vertices = this->getVertices();

		RenderEngine::get()
			->renderObject2D( m_material, m_textures, NULL, 1, vertices, 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices )
	{
		const mt::mat3f & wm = getWorldMatrix();

		//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
		//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
		//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
		//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
		const mt::vec2f & size = m_resourceVideo->getFrameSize();

		mt::vec2f transformX;
		mt::vec2f transformY;

		mt::mul_v2_m3_r( transformX, mt::vec2f( size.x, 0.0f ), wm );
		mt::mul_v2_m3_r( transformY, mt::vec2f( 0.0f, size.y ), wm );

		mt::vec2f vertices[4];

		vertices[0] = wm.v2.to_vec2f();
		vertices[1] = vertices[0] + transformX;
		vertices[2] = vertices[1] + transformY;
		vertices[3] = vertices[0] + transformY;

		for( int i = 0; i < 4; i++ )
		{
			_vertices[i].pos[0] = vertices[i].x;
			_vertices[i].pos[1] = vertices[i].y;
			_vertices[i].pos[2] = 0.f;
			_vertices[i].pos[3] = 1.f;
		}

		_vertices[0].uv[0] = 0.0f;
		_vertices[0].uv[1] = 0.0f;
		_vertices[1].uv[1] = 0.0f;
		_vertices[3].uv[0] = 0.0f;
		_vertices[1].uv[0] = 1.0f;
		_vertices[2].uv[0] = 1.0f;
		_vertices[2].uv[1] = 1.0f;
		_vertices[3].uv[1] = 1.0f;

		const ColourValue & color = getWorldColor();
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
		const Vertex2D * vertcies = getVertices();

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
}
