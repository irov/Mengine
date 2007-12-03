#	include "SoundEmitter.h"

#	include "XmlParser/XmlParser.h"

#	include "ObjectImplement.h"

#	include "ResourceSound.h"

#	include "ResourceManager.h"
#	include "LogEngine.h"

#	include "SoundEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(SoundEmitter);
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::SoundEmitter()
		: m_resource(0)
		, m_interface(0)
		, m_isHeadMode(false)
		, m_listener(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::~SoundEmitter()
	{
		if(m_interface)
		{
			m_interface->setSoundNodeListener(NULL);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Resource", "Name", m_resourcename );
			XML_CHECK_VALUE_NODE( "HeadMode", "Value", m_isHeadMode );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}


		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceSound>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG("Warning: sound emitter '%s' can't get resource '%s'\n"
				, m_name.c_str()
				, m_resourcename.c_str()
				);

			return false;
		}

		m_interface = Holder<SoundEngine>::hostage()->createSoundSource(
			m_isHeadMode,
			m_resource->get(),
			this );

		if( m_interface == 0 )
		{
			MENGE_LOG("Warning: sound emitter '%s' can't compiled\n"
				, m_name.c_str()
				);

			return false;
		}

		Holder<SoundEngine>::hostage()->registerSoundEmitter( this );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_interface );
		Holder<ResourceManager>::hostage()->releaseResource( m_resource );

		m_interface = NULL;
		m_resource = NULL;

		Holder<SoundEngine>::hostage()->unregisterSoundEmitter( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setSoundResource( const std::string & _name )
	{
		m_resourcename = _name;

		recompile();		
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setSoundListener( PyObject * _listener )
	{
		m_listener = _listener;

		registerEventListener("STOP_PLAYING", "onStopped", m_listener );
		registerEventListener("PAUSE_PLAYING", "onPaused", m_listener );
	}

	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenPaused( bool _pause )
	{
		callEvent( "PAUSE_PLAYING", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenStopped()
	{
		//Holder<SoundEngine>::hostage()->unregisterSoundEmitter( this );
		callEvent( "STOP_PLAYING", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		//Holder<SoundEngine>::hostage()->registerSoundEmitter( this );
		return m_interface->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::pause()
	{
		return m_interface->pause();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::stop()
	{
		m_interface->stop();	
		//Holder<SoundEngine>::hostage()->unregisterSoundEmitter( this );
		return ;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::isPlaying()
	{
		return m_interface->isPlaying();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setVolume( float _volume )
	{
		//float soundSourceVol = _volume * Holder<SoundEngine>::hostage()->getSoundSourceVolume();
		float commonVol = _volume * Holder<SoundEngine>::hostage()->getCommonVolume();
		return m_interface->setVolume( commonVol );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::getVolume()
	{
		return m_interface->getVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setLooped( bool _loop )
	{
		return m_interface->setLooped( _loop );
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::isLooping()
	{
		return m_interface->isLooping();
	}
	//////////////////////////////////////////////////////////////////////////
	int SoundEmitter::getLengthMs()
	{
		return m_interface->getLengthMs();
	}
	//////////////////////////////////////////////////////////////////////////
}
