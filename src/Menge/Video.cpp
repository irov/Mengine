#	include "Video.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceImageVideoStream.h"

#	include "XmlEngine.h"

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
			XML_CASE_ATTRIBUTE_NODE( "Looping", "Value", m_looping );
			XML_CASE_ATTRIBUTE_NODE( "AutoStart", "Value", m_autoStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setListener( PyObject* _listener )
	{
		Sprite::setListener( _listener );

		registerEventListener("END_VIDEO", "onVideoEnd", _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setAnimationResource( const std::string & _resource )
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
		if( Sprite::_compile() == false )
		{
			return false;
		}

		m_resourceVideo = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImageVideoStream>( m_resourceVideoName );

		if( m_resourceVideo == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_release()
	{
		Sprite::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceVideo );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::stop()
	{
		if( m_playing == true )
		{
			m_playing = false;
			callEvent( "END_VIDEO ", "(O)", this->getScript() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::pause()
	{
		m_playing = false;
		if( m_resourceVideo )
		{
			m_resourceVideo->getStream()->pause();
		}
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
		if( m_resourceVideo )
		{
			m_resourceVideo->getStream()->play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}