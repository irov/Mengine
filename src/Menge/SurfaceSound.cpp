#	include "SurfaceSound.h"

#	include "ResourceSound.h"

#	include "Interface/ResourceInterface.h"

#	include "Logger/Logger.h"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SurfaceSound::SurfaceSound()
		: m_soundBuffer(nullptr)
		, m_sourceID(0)
		, m_isHeadMode(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SurfaceSound::~SurfaceSound()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceSound::_compile()
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
			->createSoundSource( m_isHeadMode, m_soundBuffer, ESST_SOUND, streamable );

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
	void SurfaceSound::_release()
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

		m_soundBuffer = nullptr;

        m_resourceSound.release();
	}
	//////////////////////////////////////////////////////////////////////////
    bool SurfaceSound::update( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;
		//Empty

        return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::setResourceSound( const ResourceSoundPtr & _resourceSound )
	{
		if( m_resourceSound == _resourceSound )
		{
			return;
		}

		m_resourceSound = _resourceSound;

		this->recompile();
	}
    //////////////////////////////////////////////////////////////////////////
	const ResourceSoundPtr & SurfaceSound::getResourceSound() const
    {
        return m_resourceSound;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSound::getMaxSize() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSound::getSize() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceSound::getOffset() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceSound::getUVCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceSound::getUV( uint32_t _index ) const
    {
        (void)_index;

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & SurfaceSound::getColour() const
    {
        return ColourValue::identity();
    }
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_setEventListener( const pybind::dict & _listener )
	{
		//this->registerEvent( EVENT_SOUND_PAUSE, ("onSoundPause"), _listener );
		//this->registerEvent( EVENT_SOUND_END, ("onSoundEnd"), _listener );

		//this->registerEvent( EVENT_ANIMATABLE_END, ("onAnimatableEnd"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::onSoundPause( uint32_t _soundId )
	{
		(void)_soundId;

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_PAUSE )
            ->onAnimatablePause( m_enumerator );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_SOUND_PAUSE )(this);
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::onSoundStop( uint32_t _soundId )
	{
		(void)_soundId;

		this->end();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceSound::_play( float _time )
	{
        (void)_time;

		if( this->isCompile() == false )
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
	bool SurfaceSound::_restart( uint32_t _enumerator, float _time )
	{
        (void)_time;
        (void)_enumerator;
		//ToDo

		return false;	
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;

		if( m_sourceID == 0 )
		{
			return;
		}
		
		SOUND_SERVICE(m_serviceProvider)
			->pause( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_resume( uint32_t _enumerator, float _time )
	{
		(void)_time;
		(void)_enumerator;

		if( m_sourceID == 0 )
		{
			return;
		}

		SOUND_SERVICE(m_serviceProvider)
			->resume( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_stop( uint32_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SOUND_SERVICE(m_serviceProvider)
				->stop( m_sourceID );
		}

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
            ->onAnimatableStop( _enumerator );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_SOUND_END )(this, _enumerator, false);
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, false);
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_end( uint32_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SOUND_SERVICE(m_serviceProvider)
				->stop( m_sourceID );
		}

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
            ->onAnimatableEnd( _enumerator );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_SOUND_END )(this, _enumerator, true);
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, true);
	}	
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_setVolume( float _volume )
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
			->setSourceVolume( m_sourceID, m_volume, 0.f ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEmitter::setVolume invalid %s:%d %f"
				, m_resourceSound->getName().c_str()
                , m_sourceID
                , m_volume
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SurfaceSound::_getVolume() const
	{
        return m_volume;
    }
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_setLoop( bool _value )
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
	float SurfaceSound::getDuration() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float lengthMs = SOUND_SERVICE(m_serviceProvider)
			->getDuration( m_sourceID );
	
		return lengthMs;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceSound::_interrupt( uint32_t _enumerator )
	{
        (void)_enumerator;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceSound::_setTiming( float _timing )
	{
		if( this->isCompile() == false )
		{
			return;
		}

		if( m_sourceID == 0 )
		{
			return;
		}

        float lengthMs = SOUND_SERVICE(m_serviceProvider)
            ->getDuration( m_sourceID );

        float pos = _timing;
        if( _timing > lengthMs )
        {
            pos = lengthMs;
        }

		SOUND_SERVICE(m_serviceProvider)
			->setPosMs( m_sourceID, pos );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr SurfaceSound::_updateMaterial() const
	{
		return nullptr;
	}
}
