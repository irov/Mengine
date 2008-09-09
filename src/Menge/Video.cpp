#	include "Video.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceVideo.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"

namespace	Menge
{
	OBJECT_IMPLEMENT(Video)
		//////////////////////////////////////////////////////////////////////////
		Video::Video()
		: m_resourceVideo(0)
		, m_playing(false)
		, m_autoStart(false)
		, m_looping(false)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Video::loader( XmlElement * _xml )
	{
		Sprite::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("ResourceName"), MENGE_TEXT("Value"), m_resourceVideoName );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Looping"), MENGE_TEXT("Value"), m_looping );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("AutoStart"), MENGE_TEXT("Value"), m_autoStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_setListener()
	{
		Sprite::_setListener();

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
		Sprite::_update( _timing );

		if( m_playing == false )
		{
			return; 
		}

		m_timing += _timing;
		m_resourceVideo->sync( m_timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_activate()
	{
		if( Sprite::_activate() == false )
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
		Sprite::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_compile()
	{
		/*if( Sprite::_compile() == false )
		{
			return false;
		}*/

		m_resourceVideo = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceVideo>( m_resourceVideoName );

		if( m_resourceVideo == 0 )
		{
			return false;
		}

		const mt::vec2f& frameSize = m_resourceVideo->getFrameSize();
		m_renderImage = Holder<RenderEngine>::hostage()
			->createImage( m_resourceVideoName, frameSize.x, frameSize.y );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_release()
	{
		Sprite::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceVideo );

		Holder<RenderEngine>::hostage()
			->releaseImage( m_renderImage );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::stop()
	{
		if( m_playing == true )
		{
			m_playing = false;
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
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( unsigned int _debugMask )
	{
	}
	//////////////////////////////////////////////////////////////////////////
}