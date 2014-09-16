#	include "SoundEngine.h"

#	include "Interface/SoundCodecInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/CodecInterface.h"

#	include "Logger/Logger.h"

#	include "Math/utils.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundService, Menge::SoundServiceInterface, Menge::SoundEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::SoundEngine()
		: m_serviceProvider(nullptr)
		, m_muted(false)
		, m_turnStream(false)
		, m_turnSound(false)
        , m_enumerator(0)
        , m_silent(false)
		, m_supportStream(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::~SoundEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void SoundEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * SoundEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::initialize( bool _silent, bool _supportStream )
	{
        m_silent = _silent;
		m_supportStream = _supportStream;

		if( m_supportStream == true )
		{
			m_threadSoundBufferUpdate = new ThreadJob();
			m_threadSoundBufferUpdate->initialize( m_serviceProvider, 5 );

			THREAD_SERVICE(m_serviceProvider)
				->createThread( CONST_STRING_LOCAL(m_serviceProvider, "ThreadSoundBufferUpdate"), 0 );

			THREAD_SERVICE(m_serviceProvider)
				->addTask( CONST_STRING_LOCAL(m_serviceProvider, "ThreadSoundBufferUpdate"), m_threadSoundBufferUpdate );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::finalize()
	{
		this->stopSounds_();

		for( TMapSoundSource::iterator 
		    it = m_soundSourceMap.begin(), 
		    it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
        {
            SoundSourceDesc * source = m_soundSourceMap.get_value( it );

			this->stopSoundBufferUpdate_( source );

            source->source->stop();
            source->source = nullptr;

            m_poolSoundSourceDesc.destroyT( source );
        }

        m_soundSourceMap.clear();

		if( m_threadSoundBufferUpdate != nullptr )
		{
			THREAD_SERVICE(m_serviceProvider)
				->joinTask( m_threadSoundBufferUpdate );

			m_threadSoundBufferUpdate = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::supportStreamSound() const
	{
		return m_supportStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::addSoundVolumeProvider( SoundVolumeProviderInterface * _soundVolumeProvider )
	{
		m_soundVolumeProviders.push_back( _soundVolumeProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::removeSoundVolumeProvider( SoundVolumeProviderInterface * _soundVolumeProvider )
	{
		TVectorSoundVolumeProviders::iterator it_found = std::find( m_soundVolumeProviders.begin(), m_soundVolumeProviders.end(), _soundVolumeProvider );

		if( it_found == m_soundVolumeProviders.end() )
		{
			return false;
		}

		m_soundVolumeProviders.erase( it_found );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::playSounds_()
	{
		for( TMapSoundSource::iterator 
			it = m_soundSourceMap.begin(), 
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
            SoundSourceDesc * source = m_soundSourceMap.get_value( it );
			
			source->turn = true;

			if( source->state != ESS_PLAY )
			{
				continue;
			}
									
			if( source->source->play() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("SoundEngine::playSounds_ invalid play"
					);

				continue;
			}
			
			this->playSoundBufferUpdate_( source );

			this->updateSourceVolume_( source, source->volume );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void SoundEngine::pauseSounds_()
    {
        for( TMapSoundSource::iterator 
            it = m_soundSourceMap.begin(),	
            it_end = m_soundSourceMap.end();
        it != it_end;
        ++it )
        {
            SoundSourceDesc * source = m_soundSourceMap.get_value( it );
			
			source->turn = false;

			if( source->state != ESS_PLAY )
			{
				continue;
			}

			this->stopSoundBufferUpdate_( source );
            
			source->source->pause();			
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::stopSounds_()
	{
		for( TMapSoundSource::iterator 
			it = m_soundSourceMap.begin(),	
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
            SoundSourceDesc * source = m_soundSourceMap.get_value( it );

			source->turn = false;

			if( source->state != ESS_PLAY )
			{
				continue;
			}

			this->stopSoundBufferUpdate_( source );

			source->source->stop();            
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::onTurnStream( bool _turn )
	{
		if( m_turnStream == _turn )
		{
			return;
		}
		
		m_turnStream = _turn;

		if( m_turnStream == true )
		{
			this->playSounds_();	
		}
		else
		{
			this->pauseSounds_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
    void SoundEngine::onTurnSound( bool _turn )
    {
		if( m_turnSound == _turn )
		{
			return;
		}

		m_turnSound = _turn;
        
		SOUND_SYSTEM(m_serviceProvider)
            ->onTurnSound( m_turnSound );
    }
	//////////////////////////////////////////////////////////////////////////
	size_t SoundEngine::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, ESoundSourceType _type, bool _streamable )
	{
		if( m_supportStream == false && _streamable == true )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundSource: unsupport stream sound %s:%s"
				);

			return 0;
		}

		SoundSourceInterfacePtr sourceInterface = SOUND_SYSTEM(m_serviceProvider)
            ->createSoundSource( _isHeadMode, _buffer );

		if( sourceInterface == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundSource: create SoundSource invalid"
                );

			return 0;
		}

		SoundSourceDesc * source = m_poolSoundSourceDesc.createT();

		++m_enumerator;
		size_t soundId = m_enumerator;
		source->soundId = soundId;

		source->source = sourceInterface;
		source->listener = nullptr;
		source->worker = nullptr;
        source->bufferId = 0;
		
        source->timing = 0.f;
		source->volume = 1.f;

		source->state = ESS_STOP;      
		source->type = _type;

        source->streamable = _streamable;
		source->looped = false;
		source->turn = _streamable ? m_turnStream : m_turnSound;

		this->updateSourceVolume_( source, 1.f );

		m_soundSourceMap.insert( soundId, source );
        
		return soundId;
	}
    //////////////////////////////////////////////////////////////////////////
    void SoundEngine::updateSourceVolume_( SoundSourceDesc * _source, float _volume )
    {
        const SoundSourceInterfacePtr & sourceInterface = _source->source;
        
        if( this->isMute() == true || 
			(m_turnStream == false && _source->streamable == true ) || 
			(m_turnSound == false && _source->streamable == false) )
        {
            sourceInterface->setVolume( 0.f );
        }
        else
        {
            ESoundSourceType type = _source->type;

            switch( type )
            {
            case EST_SOUND:
                {
					float mixVolume = 1.f;
					mixVolume *= m_commonVolume.mixVolume();
					mixVolume *= m_soundVolume.mixVolume();
					mixVolume *= _volume;

                    sourceInterface->setVolume( mixVolume );
                }break;
            case EST_MUSIC:
                {
					float mixVolume = 1.f;
					mixVolume *= m_commonVolume.mixVolume();
					mixVolume *= m_musicVolume.mixVolume();
					mixVolume *= _volume;

                    sourceInterface->setVolume( mixVolume );
                }break;
            case EST_VOICE:
                {
					float mixVolume = 1.f;
					mixVolume *= m_commonVolume.mixVolume();
					mixVolume *= m_voiceVolume.mixVolume();
					mixVolume *= _volume;

					sourceInterface->setVolume( mixVolume );
                }break;
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr SoundEngine::createSoundBufferFromFile( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecType, bool _streamable )
	{
		if( m_supportStream == false && _streamable == true )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundBufferFromFile: unsupport stream sound %s:%s"
				, _pakName.c_str()
				, _fileName.c_str() 
				);

			return nullptr;
		}

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( _pakName, _fileName, _streamable );
        
        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundBufferFromFile: Can't open sound file %s:%s"
                , _pakName.c_str()
                , _fileName.c_str() 
                );

            return nullptr;
        }

		SoundDecoderInterfacePtr codec = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<SoundDecoderInterfacePtr>( _codecType );
		
    	if( codec == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundBufferFromFile: Can't create sound decoder for file %s:%s"
                , _pakName.c_str()
				, _fileName.c_str() 
				);

			return nullptr;
		}

		if( codec->prepareData( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundBufferFromFile: Can't initialize sound decoder for file %s:%s"
				, _pakName.c_str()
				, _fileName.c_str() 
				);

			return nullptr;
		}

		SoundBufferInterfacePtr buffer = SOUND_SYSTEM(m_serviceProvider)
            ->createSoundBuffer( codec, _streamable );

        if( buffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundBufferFromFile: Can't create sound buffer for file %s:%s"
                , _pakName.c_str()
                , _fileName.c_str() 
                );

            return nullptr;
        }

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::releaseSoundSource( size_t _sourceID )
	{
		TMapSoundSource::iterator it_find = m_soundSourceMap.find( _sourceID );
		
		if( it_find == m_soundSourceMap.end() )
		{
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::releaseSoundSource not found %d"
                , _sourceID
                );

			return false;
		}

        SoundSourceDesc * source = m_soundSourceMap.get_value( it_find );

		this->stopSoundBufferUpdate_( source );

        source->source->stop();
        source->source = nullptr;

		source->listener = nullptr;

		m_soundSourceMap.erase( it_find );
        
        m_poolSoundSourceDesc.destroyT( source );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::validSoundSource( size_t _sourceID ) const
	{
		TMapSoundSource::const_iterator it_find = m_soundSourceMap.find( _sourceID );

		if( it_find == m_soundSourceMap.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSoundVolume( const ConstString & _type, float _volume )
	{
		m_soundVolume.setVolume( _type, _volume );

		this->updateVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSoundVolume( const ConstString & _type ) const
	{
		float volume = m_soundVolume.getVolume( _type );

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////	
	void SoundEngine::setCommonVolume( const ConstString & _type, float _volume )
	{
		m_commonVolume.setVolume( _type, _volume );

		this->updateVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getCommonVolume( const ConstString & _type ) const
	{
		float volume = m_commonVolume.getVolume( _type );

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setMusicVolume( const ConstString & _type, float _volume )
	{
		m_musicVolume.setVolume( _type, _volume );

		this->updateVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getMusicVolume( const ConstString & _type ) const
	{
		float volume = m_musicVolume.getVolume( _type );

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setVoiceVolume( const ConstString & _type, float _volume )
	{
		m_voiceVolume.setVolume( _type, _volume );

		this->updateVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getVoiceVolume( const ConstString & _type ) const
	{
		float volume = m_voiceVolume.getVolume( _type );

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct SoundListenerStopDesc
		{
			SoundListenerInterface * listener;
			size_t id;
		};

		typedef stdex::vector<SoundListenerStopDesc> TVectorSoundListeners;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::update( float _timing )
	{
		TVectorSoundListeners m_listeners;

		for( TMapSoundSource::iterator 
			it = m_soundSourceMap.begin(), 
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
			SoundSourceDesc * source = m_soundSourceMap.get_value( it );

			if( source->state != ESS_PLAY )
			{
				continue;
			}

			if( source->looped == true )
			{
				continue;
			}

			if( source->turn == false )
			{
				continue;
			}
			
			//float length_ms = source->source->getLengthMs();
			//float pos_ms = source->source->getPosMs();		

			if( (source->worker != nullptr && source->worker->isDone() == true) || (source->worker == nullptr && source->timing <= 0.f) )
			{
				source->state = ESS_STOP;
				this->stopSoundBufferUpdate_( source );
				source->source->stop();
				source->timing = 0.f;

				if( source->listener != nullptr )
				{
					SoundListenerStopDesc desc;
					desc.listener = source->listener;
					desc.id = source->soundId;

					m_listeners.push_back( desc );
				}
			}
			else
			{
				source->timing -= _timing;
			}
		}

		for( TVectorSoundListeners::iterator
			it = m_listeners.begin(),
			it_end = m_listeners.end();
		it != it_end;
		++it )
		{
			const SoundListenerStopDesc & desc = *it;
			desc.listener->onSoundStop( desc.id );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::mute( bool _mute )
	{
		m_muted = _mute;

		this->updateVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::isMute() const
	{
		return m_muted;
	}
    //////////////////////////////////////////////////////////////////////////
    bool SoundEngine::isSilent() const
    {
        return m_silent;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundEngine::getSoundSourceDesc_( size_t _emitterId, SoundSourceDesc ** _desc )
    {
        bool result = m_soundSourceMap.has( _emitterId, _desc );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundEngine::getSoundSourceDesc_( size_t _emitterId, const SoundSourceDesc ** _desc ) const
    {
		bool result = m_soundSourceMap.has( _emitterId, _desc );

		return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::play( size_t _emitterId )
	{
        SoundSourceDesc * source;
		if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:play not found emitter id %d"
				, _emitterId
				);

			return false;
		}

		switch( source->state )
		{
		case ESS_STOP:
		case ESS_PAUSE:
            {
                this->stopSoundBufferUpdate_( source );
   
                float length_ms = source->source->getLengthMs();
                float pos_ms = source->source->getPosMs();

                source->timing = length_ms - pos_ms;

				source->state = ESS_PLAY;

				if( source->turn == true )
				{
					if( source->source->play() == false )
					{
						LOGGER_ERROR(m_serviceProvider)("SoundEngine::play invalid play %d"
							, _emitterId
							);

						return false;
					}
				
					this->playSoundBufferUpdate_( source );
				}
            }break;
		}

		this->updateSourceVolume_( source, source->volume );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::pause( size_t _emitterId )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:pause not found emitter id %d"
				, _emitterId
				);

			return false;
		}

		switch( source->state )
		{
		case ESS_PLAY:
			{
				source->state = ESS_PAUSE;

				if( source->turn == true )
				{
					this->stopSoundBufferUpdate_( source );
				}

				source->source->pause();
				source->timing = source->source->getPosMs();

				if( source->listener != nullptr )
				{
					source->listener->onSoundPause( source->soundId );
				}
			}
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::stop( size_t _emitterId )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:stop not found emitter id %d"
				, _emitterId
				);

			return false;
		}
        
		switch( source->state )
		{
		case ESS_PLAY:
		case ESS_PAUSE:
			{
				source->state = ESS_STOP;

				if( source->turn == true )
				{
					this->stopSoundBufferUpdate_( source );
				}

				source->source->stop();
				source->timing = source->source->getPosMs();

				if( source->listener != nullptr )
				{
					source->listener->onSoundStop( source->soundId );
				}
			}
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setLoop( size_t _emitterId, bool _looped )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setLoop not found emitter id %d"
				, _emitterId
				);

			return;
		}

		source->looped = _looped;
		source->source->setLoop( _looped );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSourceListener( size_t _emitter, SoundListenerInterface* _listener )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setSourceListener not found emitter id %d"
				, _emitter
				);

			return;
		}
	
		source->listener = _listener;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::updateVolume()
	{
		for( TMapSoundSource::iterator 
			it = m_soundSourceMap.begin(), 
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
            SoundSourceDesc * source = m_soundSourceMap.get_value( it );

            this->updateSourceVolume_( source, source->volume );
		}

		for( TVectorSoundVolumeProviders::iterator
			it = m_soundVolumeProviders.begin(),
			it_end = m_soundVolumeProviders.end();
		it != it_end;
		++it )
		{
			SoundVolumeProviderInterface * provider = *it;

			float mixSoundVolume = 1.f;
			mixSoundVolume *= m_commonVolume.mixVolume();
			mixSoundVolume *= m_soundVolume.mixVolume();
			
			float mixMusicVolume = 1.f;
			mixMusicVolume *= m_commonVolume.mixVolume();
			mixMusicVolume *= m_musicVolume.mixVolume();
							
			float mixVoiceVolume = 1.f;
			mixVoiceVolume *= m_commonVolume.mixVolume();
			mixVoiceVolume *= m_voiceVolume.mixVolume();

			provider->onSoundChangeVolume( mixSoundVolume, mixMusicVolume, mixVoiceVolume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::setSourceVolume( size_t _emitterId, float _volume )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setVolume not found emitter id %d"
				, _emitterId
				);

			return false;
		}
                
		source->volume = _volume;

        this->updateSourceVolume_( source, _volume );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::getLoop( size_t _emitterId ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getLoop not found emitter id %d"
				, _emitterId
				);

			return false;
		}

        bool looped = source->looped;

		return looped;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSourceVolume( size_t _emitterId ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getVolume not found emitter id %d"
				, _emitterId
				);

			return 0.f;
		}

        float volume = source->volume;

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getLengthMs( size_t _emitter ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getLengthMs not found emitter id %d"
				, _emitter
				);

			return 0.0f;
		}

        float ms = source->source->getLengthMs();
		
		return ms;
	}
    //////////////////////////////////////////////////////////////////////////
	bool SoundEngine::setPosMs( size_t _emitterId, float _pos )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setPosMs not found emitter id %d"
				, _emitterId
				);

			return false;
		}

        if( source->source == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine:setPosMs not setup source %d"
                , _emitterId
                );

			return false;
        }

        float current_pos = source->source->getPosMs();

		if( mt::cmp_f_f( current_pos, _pos ) == true )
        {
            return true;
        }

        float lengthMs = source->source->getLengthMs();
        
        if( _pos > lengthMs )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::setPosMs emitter %d pos %f length %f"
                , _emitterId
                , _pos
                , lengthMs
                );

            return false;
        }

        source->timing = lengthMs - _pos;

        bool hasBufferUpdate = source->worker != nullptr;

        if( hasBufferUpdate == true )
        {
            this->stopSoundBufferUpdate_( source );
        }
        
        bool playing = source->source->isPlaying();
        
        if( playing == true )
        {
            source->source->pause();
        }
        
		if( source->source->setPosMs( _pos ) == false )
        {
            return false;
        }

        if( playing == true )
        {
            if( source->source->play() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("SoundEngine::setPosMs invalid play"
                    );

                return false;
            }

			this->updateSourceVolume_( source, source->volume );
        }

        if( hasBufferUpdate == true )				
        {
            this->playSoundBufferUpdate_( source );
        }

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool SoundEngine::stopSoundBufferUpdate_( SoundSourceDesc * _source )
    {
		if( _source->streamable == false )
		{
			return false;
		}

		if( _source->worker == nullptr )
		{
			return false;
		}

		if( m_threadSoundBufferUpdate != nullptr )
		{
	        m_threadSoundBufferUpdate->removeWorker( _source->bufferId );
		}

        _source->worker = nullptr;
        _source->bufferId = 0;

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundEngine::playSoundBufferUpdate_( SoundSourceDesc * _source )
    {
		if( _source->streamable == false )
		{
			return false;				 
		}

		if( _source->worker != nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::playSoundBufferUpdate_ _source worker is not null"
				);

			return false;
		}

		if( m_threadSoundBufferUpdate != nullptr )
		{
			ThreadWorkerSoundBufferUpdatePtr worker = m_poolWorkerTaskSoundBufferUpdate.createObjectT();

			SoundBufferInterfacePtr soundBuffer = _source->source->getSoundBuffer();

			worker->initialize( m_serviceProvider, soundBuffer );

			_source->worker = worker;
			
			_source->bufferId = m_threadSoundBufferUpdate->addWorker( _source->worker );
		}
		else
		{
			_source->worker = nullptr;
			_source->bufferId = 0;
		}

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getPosMs( size_t _emitterId ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitterId, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getPosMs not found emitter id %d"
				, _emitterId
				);

			return 0.f;
		}

		if( source->source == nullptr )
		{
			return 0.f;
		}

        float pos = source->source->getPosMs();
		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
}
