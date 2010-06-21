#	include "Video.h"

#	include "ResourceManager.h"

#	include "ResourceVideo.h"
//#	include "ResourceSound.h"
#	include "SoundEmitter.h"
#	include "NodeManager.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "Logger/Logger.h"

#	include "Texture.h"
#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Video::Video()
		: m_resourceVideo( NULL )
		, m_soundEmitter( NULL )
		, m_playing(false)
		, m_autoStart(false)
		, m_looping(false)
		, m_needUpdate( false )
		, m_timing( 0.0f )
		, m_material( NULL )
		, m_resourceImage( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceVideo", "Name", m_resourceVideoName );
			XML_CASE_ATTRIBUTE_NODE( "ResourceSound", "Name", m_resourceSoundName );
			XML_CASE_ATTRIBUTE_NODE( "Looping", "Value", m_looping );
			XML_CASE_ATTRIBUTE_NODE( "AutoStart", "Value", m_autoStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_setListener( PyObject * _listener )
	{
		Node::_setListener( _listener );

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
	void Video::setLooped( bool _loop )
	{
		m_looping = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::getLooped() const
	{
		return m_looping;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_update( float _timing )
	{
		Node::_update( _timing );

		if( m_playing == false )
		{
			return; 
		}

		m_timing += _timing;
		m_needUpdate = m_resourceVideo->sync( m_timing );

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
			play_();
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
		m_resourceVideo = ResourceManager::hostage()
			->getResourceT<ResourceVideo>( m_resourceVideoName );

		if( m_resourceVideo == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Video Resource not found '%s'"
				, m_resourceVideoName.c_str() 
				);	

			return false;
		}

		m_material = Holder<RenderEngine>::hostage()
			->createMaterial();

		//m_material->textureStages = 1;
		m_material->textureStage[0].colorOp = TOP_MODULATE;

		const mt::vec2f & size = m_resourceVideo->getFrameSize();

		m_resourceImage = RenderEngine::hostage()
			->createTexture( m_resourceVideoName, size.x, size.y, Menge::PF_A8R8G8B8 );

		//m_material->textureStage[0].texture = m_resourceImage;

		if( m_resourceSoundName.empty() == false )
		{
			m_soundEmitter = Holder<NodeManager>::hostage()
				->createNodeT<SoundEmitter>( Consts::c_SoundEmitter );

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
		Holder<RenderEngine>::hostage()
			->releaseTexture( m_resourceImage );

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceVideo );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::stop()
	{
		if( m_playing == true )
		{
			m_playing = false;
			m_resourceVideo->seek( 0.0f );
			m_timing = 0.0f;
			if( m_soundEmitter && m_soundEmitter->isCompile() )
			{
				m_soundEmitter->stop();
			}
			callEvent( EVENT_VIDEO_END, "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::pause()
	{
		m_playing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		play_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::play_()
	{
		m_playing = true;
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( Camera2D * _camera )
	{
		Node::_render( _camera );
		if( m_needUpdate )
		{
			int pitch = 0;
			//Texture* renderImage = m_material->textureStage[0].texture;
			unsigned char* lockRect = m_resourceImage->lock( &pitch, false );
			m_resourceVideo->getRGBData( lockRect, pitch );
			m_resourceImage->unlock();
			m_needUpdate = false;
		}

		Vertex2D * vertices = this->getVertices();

		Holder<RenderEngine>::hostage()
			->renderObject2D( m_material, &m_resourceImage, 1, vertices, 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices )
	{
		const mt::mat3f & wm = getWorldMatrix();

		//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
		//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
		//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
		//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
		mt::vec2f transformX;
		mt::vec2f transformY;

		mt::vec2f vertices[4];

		const mt::vec2f & size = m_resourceVideo->getFrameSize();

		vertices[0] = wm.v2.to_vec2f();
		mt::mul_v2_m3_r( transformX, mt::vec2f( size.x, 0.0f ), wm );
		mt::mul_v2_m3_r( transformY, mt::vec2f( 0.0f, size.y ), wm );

		vertices[1] = vertices[0] + transformX;
		vertices[2] = vertices[1] + transformY;
		vertices[3] = vertices[0] + transformY;

		for( int i = 0; i < 4; i++ )
		{
			_vertices[i].pos[0] = vertices[i].x;
			_vertices[i].pos[1] = vertices[i].y;
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

		if( ( argb & 0xFF000000 ) == 0xFF000000 )
		{
			m_material->isSolidColor = true;
		}
		else
		{
			m_material->isSolidColor = false;
		}
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
		Vertex2D * vertcies = getVertices();

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
		invalidateVertices();
	}
}
