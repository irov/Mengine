#	include "Video.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceVideo.h"
//#	include "ResourceSound.h"
#	include "SoundEmitter.h"
#	include "SceneManager.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "LogEngine.h"

#	include "ResourceImageDefault.h"
#	include "Texture.h"
namespace	Menge
{
	OBJECT_IMPLEMENT(Video)
		//////////////////////////////////////////////////////////////////////////
		Video::Video()
		: Node()
		, m_resourceVideo( NULL )
		, m_soundEmitter( NULL )
		, m_playing(false)
		, m_autoStart(false)
		, m_looping(false)
		, m_needUpdate( false )
		, m_timing( 0.0f )
		, m_renderObject( NULL )
		, m_size( 0.0f, 0.0f )
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
	void Video::_setListener()
	{
		Node::_setListener();

		registerEvent( EVENT_VIDEO_END, ("onVideoEnd"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setAnimationResource( const String& _resource )
	{
		m_resourceVideoName = _resource;
		recompile();
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
		m_resourceVideo = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceVideo>( m_resourceVideoName );

		if( m_resourceVideo == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Video Resource not found \"%s\"",
							m_resourceVideoName.c_str() );	
			return false;
		}

		m_renderObject = Holder<RenderEngine>::hostage()
							->createRenderObject();

		m_renderObject->material.primitiveType = PT_TRIANGLELIST;

		m_renderObject->vertices.resize( 4 );
		m_renderObject->material.indicies.resize( 6 );
		m_renderObject->material.indicies[0] = 0;
		m_renderObject->material.indicies[1] = 3;
		m_renderObject->material.indicies[2] = 1;
		m_renderObject->material.indicies[3] = 1;
		m_renderObject->material.indicies[4] = 3;
		m_renderObject->material.indicies[5] = 2;

		m_renderObject->material.textureStages = 1;

		m_renderObject->vertices[0].uv[0] = 
		m_renderObject->vertices[0].uv[1] = 
		m_renderObject->vertices[1].uv[1] = 
		m_renderObject->vertices[3].uv[0] = 0.0f;
		m_renderObject->vertices[1].uv[0] = 
		m_renderObject->vertices[2].uv[0] = 
		m_renderObject->vertices[2].uv[1] = 
		m_renderObject->vertices[3].uv[1] = 1.0f;

		m_size = m_resourceVideo->getFrameSize();
		//m_renderImage = Holder<RenderEngine>::hostage()
		//	->createImage( m_resourceVideoName, m_size.x, m_size.y, Menge::PF_A8R8G8B8 );
		ResourceFactoryParam param;
		param.name = m_resourceVideoName + "Texture";
		ResourceImageDefault* resource = new ResourceImageDefault( param );
		resource->createImageFrame_( "CreateImage", m_size );

		Holder<ResourceManager>::hostage()
			->registerResource( resource );

		m_resourceImage = Holder<ResourceManager>::hostage()
							->getResourceT<ResourceImage>( m_resourceVideoName + "Texture" );
		m_renderObject->material.textureStage[0].texture = m_resourceImage->getImage( 0 );
			
		if( m_resourceSoundName.empty() == false )
		{
			m_soundEmitter = Holder<SceneManager>::hostage()
				->createNodeT<SoundEmitter>( "SoundEmitter" );

			addChildren( m_soundEmitter );
			m_soundEmitter->setSoundResource( m_resourceSoundName );
			if( m_soundEmitter->compile() == false )
			{
				MENGE_LOG_ERROR( "Warning: video failed to compile sound resource \"%s\""
					, m_resourceSoundName.c_str() );
			}
		}

		updateVertices_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_release()
	{

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceImage );

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
	void Video::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );
		if( m_needUpdate )
		{
			int pitch = 0;
			Texture* renderImage = m_renderObject->material.textureStage[0].texture;
			unsigned char* lockRect = renderImage->lock( &pitch, false );
			m_resourceVideo->getRGBData( lockRect, pitch );
			renderImage->unlock();
			m_needUpdate = false;
		}

		updateVertices_();

		if( m_invalidateColor == true )
		{
			uint32 argb = getWorldColor().getAsARGB();
			RenderObject::ApplyColor applyColor( argb );
			std::for_each( m_renderObject->vertices.begin(), m_renderObject->vertices.end(),
							applyColor );

			if( ( argb & 0xFF000000 ) == 0xFF000000 )
			{
				m_renderObject->material.isSolidColor = true;
			}
			else
			{
				m_renderObject->material.isSolidColor = false;
			}
			//m_renderObject->material.color = getWorldColor();
		}

		Holder<RenderEngine>::hostage()->renderObject( m_renderObject );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::updateVertices_()
	{
		const mt::mat3f & wm = getWorldMatrix();

		//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
		//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
		//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
		//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
		mt::vec2f transformX;
		mt::vec2f transformY;

		mt::vec2f vertices[4];

		vertices[0] = wm.v2.to_vec2f();
		mt::mul_v2_m3_r( transformX, mt::vec2f( m_size.x, 0.0f ), wm );
		mt::mul_v2_m3_r( transformY, mt::vec2f( 0.0f, m_size.y ), wm );

		vertices[1] = vertices[0] + transformX;
		vertices[2] = vertices[1] + transformY;
		vertices[3] = vertices[0] + transformY;

		for( int i = 0; i < 4; i++ )
		{
			m_renderObject->vertices[i].pos[0] = vertices[i].x;
			m_renderObject->vertices[i].pos[1] = vertices[i].y;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		if( m_renderObject != NULL )
		{
			updateVertices_();
		}
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		mt::vec2f v( m_renderObject->vertices[0].pos[0], m_renderObject->vertices[0].pos[1] );
		mt::reset( _boundingBox, v );

		for( int i = 1; i != 4; ++i )
		{
			v.x = m_renderObject->vertices[i].pos[0];
			v.y = m_renderObject->vertices[i].pos[1];
			mt::add_internal_point( _boundingBox, v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
