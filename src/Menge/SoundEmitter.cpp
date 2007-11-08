#	include "SoundEmitter.h"

#	include "XmlParser/XmlParser.h"

#	include "ObjectImplement.h"

#	include "ResourceSound.h"

#	include "ResourceManager.h"

#	include "SoundEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(SoundEmitter);
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::SoundEmitter()
		: m_resourceSound(0)
		, m_interface(0)
		, m_isHeadMode(false)
		, m_listener(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::~SoundEmitter()
	{
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
			XML_CHECK_VALUE_NODE( "Resource", "Name", m_resourceName );
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

		m_resourceSound = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceSound>( m_resourceName );

		if( m_resourceSound == 0 )
		{
			printf("Warning: sound emitter '%s' can't get resource '%s'\n"
				, m_name.c_str()
				, m_resourceName.c_str()
				);

			return false;
		}

		m_interface = Holder<SoundEngine>::hostage()->createSoundSource(m_isHeadMode,m_resourceSound->get(),this);

		if( m_interface == 0 )
		{
			printf("Warning: sound emitter '%s' can't compiled\n"
				, m_name.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_release()
	{
		Holder<ResourceManager>::hostage()->releaseResource( m_resourceSound );
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setSoundResource( const std::string & _name )
	{
		m_resourceName = _name;

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
	void SoundEmitter::listenPaused(bool _pause)
	{
		callEvent( "PAUSE_PLAYING", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenStopped()
	{
		Holder<SoundEngine>::hostage()->unregisterSoundEmitter( this );
		callEvent( "STOP_PLAYING", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		Holder<SoundEngine>::hostage()->registerSoundEmitter( this );
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
		Holder<SoundEngine>::hostage()->unregisterSoundEmitter( this );
		return m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::isPlaying()
	{
		return m_interface->isPlaying();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setVolume( float _vol )
	{
		float soundSourceVol = _vol * Holder<SoundEngine>::hostage()->getSoundSourceVolume();
		float commonVol = soundSourceVol * Holder<SoundEngine>::hostage()->getCommonVolume();
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
	int SoundEmitter::getLengthMS()
	{
		return m_interface->getLengthMS();
	}
	//////////////////////////////////////////////////////////////////////////
}