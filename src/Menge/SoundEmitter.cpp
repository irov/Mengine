#	include "SoundEmitter.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "Application.h"

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
		, m_volume( 1.0f )
		, m_looped( false )
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
		bool enabled = Holder<Application>::hostage()->getSoundEnabled();
		if( !enabled || Node::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Resource"), MENGE_TEXT("Name"), m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("HeadMode"), MENGE_TEXT("Value"), m_isHeadMode );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Looping"), MENGE_TEXT("Value"), m_looped );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceSound>( m_resourcename );

		if( m_resource == 0 )
		{
			/*MENGE_LOG( MENGE_TEXT("Warning: sound emitter '%s' can't get resource '%s'")
				, m_name.c_str()
				, m_resourcename.c_str()
				);*/

			return false;
		}

		SoundBufferInterface * soundBuffer = m_resource->get();

		m_interface = Holder<SoundEngine>::hostage()
			->createSoundSource( 
			m_isHeadMode
			, soundBuffer
			, this 
			);

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Warning: sound emitter '%s' can't compiled")
				, m_name.c_str()
				);

			return false;
		}

		m_interface->setLooped( m_looped );
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
	void SoundEmitter::setSoundResource( const String& _name )
	{
		m_resourcename = _name;

		//recompile();		
		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceSound>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Warning: sound emitter '%s' can't get resource '%s'")
				, m_name.c_str()
				, m_resourcename.c_str()
				);

			return;
		}
		m_interface->setSoundBuffer( m_resource->get() );
	}
	//////////////////////////////////////////////////////////////////////////
	/*void SoundEmitter::setSoundListener( PyObject * _listener )
	{
		m_listener = _listener;

		registerEvent("STOP_PLAYING", "onStopped", m_listener );
		registerEvent("PAUSE_PLAYING", "onPaused", m_listener );
	}*/

	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenPaused( bool _pause )
	{
		callEvent( EVENT_SOUND_PAUSE, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenStopped()
	{
		//Holder<SoundEngine>::hostage()->unregisterSoundEmitter( this );
		callEvent( EVENT_SOUND_STOP, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		//Holder<SoundEngine>::hostage()->registerSoundEmitter( this );
		//printf("playing %s looped = %d\n", m_resourcename.c_str(), m_looped );
		//MENGE_LOG( MENGE_TEXT("playing %s"), m_resourcename.c_str());
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
		//MENGE_LOG("stopping %s", m_resourcename.c_str());
		if( m_interface )
		{
			m_interface->stop();	
		}
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
		m_volume = _volume;
		float commonVol = m_volume * Holder<SoundEngine>::hostage()->getCommonVolume();
		return m_interface->setVolume( commonVol );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::updateVolume()
	{
		setVolume( m_volume );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::getVolume()
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setLooped( bool _loop )
	{
		m_looped = _loop;
		if( m_interface )
		{
			m_interface->setLooped( m_looped );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::isLooping()
	{
		return m_looped;
	}
	//////////////////////////////////////////////////////////////////////////
	int SoundEmitter::getLengthMs()
	{
		return m_interface->getLengthMs();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setListener()
	{
		Node::_setListener();

		registerEvent( EVENT_SOUND_STOP, ("onStopped"), m_listener );
		registerEvent( EVENT_SOUND_PAUSE, ("onPaused"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
}
