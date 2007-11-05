#	include "SoundGroup.h"

#	include "XmlParser/XmlParser.h"

#	include "ObjectImplement.h"

#	include "ResourceSound.h"

#	include "ResourceManager.h"

#	include "SoundEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(SoundGroup);
	//////////////////////////////////////////////////////////////////////////
	SoundGroup::SoundGroup()
		: m_resourceSound(0)
		, m_interface(0)
		, m_isHeadMode(false)
		, m_listener(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundGroup::~SoundGroup()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::_changePivot()
	{
		const mt::vec2f & pos = getLocalPosition();
		m_interface->setPosition(pos.x,pos.y,0);
		m_changePivot = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundGroup::listenPaused( bool _pause )
	{
		callEvent( "PAUSE_PLAYING", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundGroup::listenStopped()
	{
		callEvent( "STOP_PLAYING", "(O)", this->getScript() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::play( const std::string & _name )
	{
		return m_interface->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::pause( const std::string & _name )
	{
		return m_interface->pause();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::stop( const std::string & _name )
	{
		return m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundGroup::isPlaying( const std::string & _name ) const
	{
		return m_interface->isPlaying();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::setVolume(float vol)
	{
		return m_interface->setVolume(vol);
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundGroup::getVolume() const
	{
		return m_interface->getVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::setLooped(bool loop)
	{
		return m_interface->setLooped(loop);
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundGroup::isLooping() const
	{
		return m_interface->isLooping();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundGroup::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundGroup::_compile()
	{
		m_resourceSound = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceSound>( m_resourceName );

		m_interface = Holder<SoundEngine>::hostage()->createSoundSource(m_isHeadMode,m_resourceSound->get(),this);

		if( m_resourceSound == 0 )
		{
			printf("can't compile SoundGroup");
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::setSoundResource( const std::string & _name )
	{
		m_resourceName = _name;

		if( m_resourceSound )
		{
			Holder<ResourceManager>::hostage()->releaseResource( m_resourceSound );
			Holder<SoundEngine>::hostage()->releaseSoundSource( m_interface );
		}
		
		m_resourceSound = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceSound>( m_resourceName );

		m_interface = Holder<SoundEngine>::hostage()->createSoundSource(m_isHeadMode,m_resourceSound->get(),this);

		if( m_resourceSound == 0 )
		{
			printf("can't set resource in SoundGroup");
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::setSoundListener( PyObject * _listener )
	{
		m_listener = _listener;

		registerEventListener("STOP_PLAYING", "onStopped", m_listener );
		registerEventListener("PAUSE_PLAYING", "onPaused", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::_release()
	{
		Holder<ResourceManager>::hostage()->releaseResource( m_resourceSound );
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundGroup::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Resource", "Name", m_resourceName );
			XML_CHECK_VALUE_NODE( "HeadMode", "Value", m_isHeadMode );
		}
	}
}