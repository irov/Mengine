#	include "SoundEmitter.h"

#	include "ResourceSound.h"

#	include "Interface/ResourceInterface.h"

#	include "Logger/Logger.h"

//#	include "SoundEngine.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::SoundEmitter()
		: m_soundBuffer(nullptr)
		, m_sourceID(0)
		, m_isHeadMode(false)
		, m_onSoundPauseEvent(false)
		, m_onSoundStopEvent(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::~SoundEmitter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_activate()
	{
		if( Node::_activate() == false )
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
	bool SoundEmitter::_compile()
	{
		if( m_resourceSound == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_compile: '%s' resource is null"
				, this->getName().c_str()				
				);

			return false;
		}

        if( m_resourceSound.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_compile: '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceSound->getName().c_str()
                );

            return false;
        }

		m_soundBuffer = m_resourceSound->createSoundBuffer();

        if( m_soundBuffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_compile: '%s' sound buffer not create"
                , this->getName().c_str() 
                );

            return false;
        }

		bool streamable = m_resourceSound->isStreamable();

		m_sourceID = SOUND_SERVICE(m_serviceProvider)
			->createSoundSource( m_isHeadMode, m_soundBuffer, EST_SOUND, streamable );

		if( m_sourceID == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_compile: sound emitter '%s' not compiled"
				, this->getName().c_str() 
				);

			return false;
		}

		SOUND_SERVICE(m_serviceProvider)
			->setSourceListener( m_sourceID, this );

		SOUND_SERVICE(m_serviceProvider)
			->setLoop( m_sourceID, m_loop );

		float volume = m_resourceSound->getDefaultVolume();
		this->setVolume( volume );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_release()
	{ 
		if( SOUND_SERVICE(m_serviceProvider)
			->releaseSoundSource( m_sourceID ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_release %s emitter invalid release sound %d"
				, this->getName().c_str()
				, m_sourceID
				);
		}

		m_sourceID = 0;

		if( m_soundBuffer != nullptr )
		{
			m_resourceSound->destroySoundBuffer( m_soundBuffer );
			m_soundBuffer = nullptr;
		}

        m_resourceSound.release();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setResourceSound( ResourceSound * _resourceSound )
	{
		if( m_resourceSound == _resourceSound )
		{
			return;
		}

		m_resourceSound = _resourceSound;

		this->recompile();
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceSound * SoundEmitter::getResourceSound() const
    {
        return m_resourceSound;
    }
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::onSoundPause( uint32_t _soundId )
	{
		(void)_soundId;

		if( m_onSoundPauseEvent == true )
		{
            EVENTABLE_CALL(m_serviceProvider, this, EVENT_SOUND_PAUSE )( "(O)", this->getEmbed() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::onSoundStop( uint32_t _soundId )
	{
		(void)_soundId;

		this->end();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_play( float _time )
	{
        (void)_time;

		if( this->isActivate() == false )
		{
			return false;
		}

		if( m_sourceID == 0 )
		{
			return false;
		}
        
		if( SOUND_SERVICE(m_serviceProvider)
			->play( m_sourceID ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_play %s invalid play [%d] resource %s"
                , this->getName().c_str()
                , m_sourceID
                , m_resourceSound->getName().c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_restart( float _time, uint32_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;
		//ToDo

		return false;	
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_pause( uint32_t _enumerator )
	{
		if( m_sourceID == 0 )
		{
			return;
		}
		
		SOUND_SERVICE(m_serviceProvider)
			->pause( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_resume( uint32_t _enumerator )
	{
		if( m_sourceID == 0 )
		{
			return;
		}

		SOUND_SERVICE(m_serviceProvider)
			->resume( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_stop( uint32_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SOUND_SERVICE(m_serviceProvider)
				->stop( m_sourceID );
		}

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_SOUND_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );		
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_end( uint32_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SOUND_SERVICE(m_serviceProvider)
				->stop( m_sourceID );
		}

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_SOUND_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}	
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setVolume( float _volume )
	{
        if( fabsf( m_volume - _volume ) < 0.00001f )
        {
            return;
        }

        m_volume = _volume;

		if( m_sourceID == 0 )
		{
			return;
		}        

		if( SOUND_SERVICE(m_serviceProvider)
			->setSourceVolume( m_sourceID, m_volume ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEmitter::setVolume invalid %s:%d %f"
				, m_resourceSound->getName().c_str()
                , m_sourceID
                , m_volume
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::_getVolume() const 
	{
        return m_volume;
    }
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setLoop( bool _value )
	{
        (void)_value;

		if( m_sourceID == 0 )
		{
			return;
		}
		
		SOUND_SERVICE(m_serviceProvider)
			->setLoop( m_sourceID, m_loop );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::getLengthMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float lengthMs = SOUND_SERVICE(m_serviceProvider)
			->getLengthMs( m_sourceID );
	
		return lengthMs;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_SOUND_PAUSE, ("onSoundPause"), _listener, &m_onSoundPauseEvent );
		this->registerEvent( EVENT_SOUND_END, ("onSoundEnd"), _listener, &m_onSoundStopEvent );
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_interrupt( uint32_t _enumerator )
	{
        (void)_enumerator;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setTiming( float _timing )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		if( m_sourceID == 0 )
		{
			return;
		}

        float lengthMs = SOUND_SERVICE(m_serviceProvider)
            ->getLengthMs( m_sourceID );

        float pos = _timing;
        if( _timing > lengthMs )
        {
            pos = lengthMs;
        }

		SOUND_SERVICE(m_serviceProvider)
			->setPosMs( m_sourceID, pos );
	}
	//////////////////////////////////////////////////////////////////////////
}
