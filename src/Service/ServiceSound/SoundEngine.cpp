#	include "SoundEngine.h"

#	include "Interface/SoundCodecInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundService, Menge::SoundServiceInterface, Menge::SoundEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::SoundEngine()
		: m_serviceProvider(nullptr)
		, m_muted(false)
		, m_turn(false)
        , m_enumerator(0)
        , m_silent(false)
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
	bool SoundEngine::initialize( bool _silent )
	{
        m_silent = _silent;

        m_threadSoundBufferUpdate.initialize( m_serviceProvider, 50 );

        THREAD_SERVICE(m_serviceProvider)
            ->addTask( &m_threadSoundBufferUpdate, 0 );        

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

            if( source->taskSoundBufferUpdate != nullptr )
            {
                this->stopSoundBufferUpdate_( source );
                //desc->taskSoundBufferUpdate->stop();

                //THREAD_SERVICE(m_serviceProvider)
                //    ->joinTask( desc->taskSoundBufferUpdate );

                //desc->taskSoundBufferUpdate->destroy();
                //desc->taskSoundBufferUpdate = nullptr;
            }

            source->soundSourceInterface->stop();
            source->soundSourceInterface->destroy();

            m_poolSoundSourceDesc.destroyT( source );
        }

        m_soundSourceMap.clear();

        THREAD_SERVICE(m_serviceProvider)
            ->cancelTask( &m_threadSoundBufferUpdate );  
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

			if( source->state == ESS_PLAYING )
			{
				if( source->soundSourceInterface->play() == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("SoundEngine::playSounds_ invalid play"
                        );

                    continue;
                }

				if( source->streamable == true && source->taskSoundBufferUpdate == nullptr )
				{
                    this->playSoundBufferUpdate_( source );
				}
			}
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

            switch( source->state )
            {
            case ESS_PLAYING:
                {
                    if( source->taskSoundBufferUpdate != nullptr )
                    {
                        this->stopSoundBufferUpdate_( source );
                    }

                    source->soundSourceInterface->pause();
                }
            case ESS_STOPPING:
                {
                    if( source->taskSoundBufferUpdate != nullptr )
                    {
                        this->stopSoundBufferUpdate_( source );
                    }
                }
            }
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

			switch( source->state )
            {
            case ESS_PLAYING:
                {
                    if( source->taskSoundBufferUpdate != nullptr )
                    {
                        this->stopSoundBufferUpdate_( source );
                    }

                    source->soundSourceInterface->stop();
                }
            case ESS_STOPPING:
                {
                    if( source->taskSoundBufferUpdate != nullptr )
                    {
                        this->stopSoundBufferUpdate_( source );
                    }
                }
            }
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::onTurnStream( bool _turn )
	{
		m_turn = _turn;

		if( m_turn == true )
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
		m_turn = _turn;
        
		SOUND_SYSTEM(m_serviceProvider)
            ->onTurnSound( m_turn );

		this->updateVolume_();
    }
	//////////////////////////////////////////////////////////////////////////
	unsigned int SoundEngine::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, ESoundSourceType _type, bool _streamable )
	{
		SoundSourceInterface * sourceInterface = SOUND_SYSTEM(m_serviceProvider)
            ->createSoundSource( _isHeadMode, _sample );

		if( sourceInterface == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundSource: create SoundSource invalid"
                );

			return 0;
		}

		++m_enumerator;
		unsigned int soundId = m_enumerator;

		SoundSourceDesc * source = m_poolSoundSourceDesc.createT();

		source->soundSourceInterface = sourceInterface;
		source->listener = nullptr;
		source->taskSoundBufferUpdate = nullptr;
        source->taskSoundBufferUpdateId = 0;
		
        source->timing = 0.f;
		source->volume = 1.f;

		source->state = ESS_STOPPED;        
		source->type = _type;

        source->streamable = _streamable;
		source->looped = false;

		TMapSoundSource::iterator it_insert = 
            m_soundSourceMap.insert( soundId, source ).first;
        
        this->updateSourceVolume_( source, 1.f );

		return soundId;
	}
    //////////////////////////////////////////////////////////////////////////
    void SoundEngine::updateSourceVolume_( SoundSourceDesc * _source, float _volume )
    {
        SoundSourceInterface * sourceInterface = _source->soundSourceInterface;
        
        if( this->isMute() == true || m_turn == false )
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
	SoundBufferInterface * SoundEngine::createSoundBufferFromFile( const ConstString& _pakName, const FilePath & _filename, const ConstString & _codecType, bool _isStream )
	{
		SoundDesc desc;

		desc.stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( _pakName, _filename );
        
        if( desc.stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundEngine::createSoundBufferFromFile: Can't open sound file %s:%s"
                , _pakName.c_str()
                , _filename.c_str() 
                );

            return nullptr;
        }

		desc.codec = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<SoundDecoderInterfacePtr>( _codecType, desc.stream );
		
    	if( desc.codec == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "SoundEngine::createSoundBufferFromFile: Can't create sound decoder for file %s:%s"
                , _pakName.c_str()
				, _filename.c_str() 
				);

			return nullptr;
		}

		SoundBufferInterface* sample = SOUND_SYSTEM(m_serviceProvider)
            ->createSoundBuffer( desc.codec, _isStream );

        if( sample == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundEngine::createSoundBufferFromFile: Can't create sound buffer for file %s:%s"
                , _pakName.c_str()
                , _filename.c_str() 
                );

            return nullptr;
        }

		if( _isStream == false )
		{
			desc.codec = nullptr;
		}

		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundSource( unsigned int _sourceID )
	{
		TMapSoundSource::iterator it_find = m_soundSourceMap.find( _sourceID );
		
		if( it_find == m_soundSourceMap.end() )
		{
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::releaseSoundSource not found %d"
                , _sourceID
                );

			return;
		}

        SoundSourceDesc * source = m_soundSourceMap.get_value( it_find );

        if( source->taskSoundBufferUpdate != nullptr )
        {
            this->stopSoundBufferUpdate_( source );
        }

        source->soundSourceInterface->stop();
        source->soundSourceInterface->destroy();

		m_soundSourceMap.erase( it_find );
        
        m_poolSoundSourceDesc.destroyT( source );
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::validSoundSource( unsigned int _sourceID ) const
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

		this->updateVolume_();
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

		this->updateVolume_();
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

		this->updateVolume_();
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

		this->updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getVoiceVolume( const ConstString & _type ) const
	{
		float volume = m_voiceVolume.getVolume( _type );

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::update( float _timing )
	{
		//if( m_turn == false )
		//{
		//	return;
		//}

		for( TMapSoundSource::iterator 
			it = m_soundSourceMap.begin(), 
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
			SoundSourceDesc * source = m_soundSourceMap.get_value( it );

			switch( source->state )
			{
			case ESS_STOPPED:
			case ESS_PAUSED:
                {
                }break;
			case ESS_STOP_PLAY:
			case ESS_PAUSE_PLAY:
                {
                }break;
            case ESS_PLAYING:
                {
                    if( source->looped == false )
                    {
                        source->timing -= _timing;

                        if( source->timing <= 0.f )
                        {
                            if( source->streamable == true && source->taskSoundBufferUpdate != nullptr )
                            {
                                source->state = ESS_STOPPED;
                                this->stopSoundBufferUpdate_( source );
                                //source->taskSoundBufferUpdate->stop();
                            }
                            else
                            {
                                source->state = ESS_STOPPED;
                                source->soundSourceInterface->stop();
                            }

                            if( source->listener != nullptr )
                            {
                                m_stopListeners.push_back( source->listener );
                            }
                        }
                    }
                }break;
			case ESS_STOPPING:
                {
                    if( source->streamable == true && source->taskSoundBufferUpdate != nullptr )
                    {
                        this->stopSoundBufferUpdate_( source );
                        //source->taskSoundBufferUpdate->stop();
                    }
                    else
                    {
                        source->state = ESS_STOPPED;
                        source->soundSourceInterface->stop();
                    }

                    if( source->listener != nullptr )
                    {
                        m_stopListeners.push_back( source->listener );
                    }
                }break;
            case ESS_PAUSING:
                {
                    if( source->streamable == true && source->taskSoundBufferUpdate != nullptr )
                    {
                        this->stopSoundBufferUpdate_( source );
                        //source->taskSoundBufferUpdate->stop();
                    }
                    else
                    {
                        source->state = ESS_PAUSED;
                        source->soundSourceInterface->pause();
                    }

                    if( source->listener != nullptr )
                    {
                        m_pauseListeners.push_back( source->listener );
                    }
                }break;
            case ESS_NEED_RESTART:
                {
                    //if( source.streamable == true && source.taskSoundBufferUpdate != NULL )
                    //{
                    //    break;	// can't restart until stopped
                    //}

                    if( source->taskSoundBufferUpdate != nullptr )
                    {
                        this->stopSoundBufferUpdate_( source );
                    }

                    source->state = ESS_PLAYING;
                    source->timing = source->soundSourceInterface->getLengthMs();
                    source->timing -= source->soundSourceInterface->getPosMs();
                    source->soundSourceInterface->stop();

                    if( source->soundSourceInterface->play() == false )
                    {
                        LOGGER_ERROR(m_serviceProvider)("SoundEngine::update ESS_NEED_RESTART invalid play"
                            );

                        continue;
                    }

                    if( source->streamable == true )
                    {
                        this->playSoundBufferUpdate_( source );
                    }

                    //if( source->listener != nullptr )
                    //{
                    //    m_stopListeners.push_back( source->listener );
                    //}
                }break;
			}
		}

		// listeners should not be changed here
		// e.g. setSourceListener( ... ) should not be called before this moment
		for( TVectorSourceListener::iterator 
			it = m_stopListeners.begin(), 
			it_end = m_stopListeners.end();
		it != it_end;
		++it )
		{
			(*it)->listenSoundNodeStopped();
		}
	
		m_stopListeners.clear();

		for( TVectorSourceListener::iterator 
			it = m_pauseListeners.begin(), 
			it_end = m_pauseListeners.end();
		it != it_end;
		++it )
		{
			(*it)->listenSoundNodePaused();
		}
	
		m_pauseListeners.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::mute( bool _mute )
	{
		m_muted = _mute;

		this->updateVolume_();
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
    bool SoundEngine::getSoundSourceDesc_( unsigned int _emitterId, SoundSourceDesc ** _desc )
    {
        bool result = m_soundSourceMap.has( _emitterId, _desc );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundEngine::getSoundSourceDesc_( unsigned int _emitterId, const SoundSourceDesc ** _desc ) const
    {
		bool result = m_soundSourceMap.has( _emitterId, _desc );

		return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::play( unsigned int _emitter )
	{
        SoundSourceDesc * source;
		if( this->getSoundSourceDesc_( _emitter, &source ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:play not found emitter id %d"
				, _emitter
				);

			return false;
		}

		switch( source->state )
		{
        case ESS_PAUSED:
		case ESS_STOPPED:
            {
                if( source->taskSoundBufferUpdate != nullptr )
                {
                    this->stopSoundBufferUpdate_( source );
                }

                float length_ms = source->soundSourceInterface->getLengthMs();
                float pos_ms = source->soundSourceInterface->getPosMs();

                source->timing = length_ms - pos_ms;

                if( source->soundSourceInterface->play() == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("SoundEngine::play invalid play"
                        );

                    return false;
                }

                source->state = ESS_PLAYING;

                if( source->streamable == true )				
                {
                    this->playSoundBufferUpdate_( source );
                }
            }break;
		case ESS_STOPPING:
			source->state = ESS_NEED_RESTART;
			break;
		case ESS_PAUSING:
			source->state = ESS_PLAYING;
			break;
		case ESS_STOP_PLAY:
		case ESS_PAUSE_PLAY:
		case ESS_PLAYING:
		case ESS_NEED_RESTART:
			// nothing to do
			break;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::pause( unsigned int _emitter )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:pause not found emitter id %d"
				, _emitter
				);

			return false;
		}

		switch( source->state )
		{
		case ESS_STOPPED:
		case ESS_PAUSED:
		case ESS_STOPPING:
		case ESS_PAUSING:
			// nothing to do
			break;
		case ESS_STOP_PLAY:
			source->state = ESS_STOPPED;
			break;
		case ESS_PAUSE_PLAY:
			source->state = ESS_PAUSED;
			break;
		case ESS_PLAYING:
			source->state = ESS_PAUSING;
			break;
		case ESS_NEED_RESTART:
			source->state = ESS_STOPPING;
			break;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::stop( unsigned int _emitter )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:stop not found emitter id %d"
				, _emitter
				);

			return false;
		}
        
        switch( source->state )
		{
		case ESS_STOPPED:
		case ESS_STOPPING:
			// nothing to do
			break;
		case ESS_STOP_PLAY:
			source->state = ESS_STOPPED;
			break;
		case ESS_PLAYING:
		case ESS_NEED_RESTART:
		case ESS_PAUSED:
		case ESS_PAUSING:
		case ESS_PAUSE_PLAY:
			source->state = ESS_STOPPING;
			break;
		}	

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setLoop( unsigned int _emitter, bool _looped )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setLoop not found emitter id %d"
				, _emitter
				);

			return;
		}

		source->looped = _looped;
		source->soundSourceInterface->setLoop( _looped );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSourceListener( unsigned int _emitter, SoundNodeListenerInterface* _listener )
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
	void SoundEngine::updateVolume_()
	{
		for( TMapSoundSource::iterator 
			it = m_soundSourceMap.begin(), 
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
            SoundSourceDesc * source = it->second;

            this->updateSourceVolume_( source, source->volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::setSourceVolume( unsigned int _emitter, float _volume )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setVolume not found emitter id %d"
				, _emitter
				);

			return false;
		}
                
		source->volume = _volume;

        this->updateSourceVolume_( source, _volume );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::getLoop( unsigned int _emitter ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getLoop not found emitter id %d"
				, _emitter
				);

			return false;
		}

        bool looped = source->looped;

		return looped;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSourceVolume( unsigned int _emitter ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getVolume not found emitter id %d"
				, _emitter
				);

			return 0.f;
		}

        float volume = source->volume;

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getLengthMs( unsigned int _emitter ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getLengthMs not found emitter id %d"
				, _emitter
				);

			return 0.0f;
		}

        float ms = source->soundSourceInterface->getLengthMs();
		
		return ms;
	}
    //////////////////////////////////////////////////////////////////////////
	bool SoundEngine::setPosMs( unsigned int _emitter, float _pos )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setPosMs not found emitter id %d"
				, _emitter
				);

			return false;
		}

        if( source->soundSourceInterface == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine:setPosMs not setup source %d"
                , _emitter
                );

            return false;
        }

        float current_pos = source->soundSourceInterface->getPosMs();

        if( fabsf( current_pos - _pos ) < 0.0001f )
        {
            return true;
        }

        float lengthMs = source->soundSourceInterface->getLengthMs();
        
        if( _pos > lengthMs )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::setPosMs emitter %d pos %f length %f"
                , _emitter
                , _pos
                , lengthMs
                );

            return false;
        }

        source->timing = lengthMs - _pos;

        bool hasBufferUpdate = source->taskSoundBufferUpdate != nullptr;

        if( hasBufferUpdate == true )
        {
            this->stopSoundBufferUpdate_( source );
        }
        
        bool playing = source->soundSourceInterface->isPlaying();
        
        if( playing == true )
        {
            source->soundSourceInterface->pause();
        }
        
		if( source->soundSourceInterface->setPosMs( _pos ) == false )
        {
            return false;
        }

        if( playing == true )
        {
            if( source->soundSourceInterface->play() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("SoundEngine::setPosMs invalid play"
                    );

                return false;
            }
        }

        if( hasBufferUpdate == true )				
        {
            this->playSoundBufferUpdate_( source );
        }

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void SoundEngine::stopSoundBufferUpdate_( SoundSourceDesc * _source )
    {
        m_threadSoundBufferUpdate.removeWorker( _source->taskSoundBufferUpdateId );

        _source->taskSoundBufferUpdate = nullptr;
        _source->taskSoundBufferUpdateId = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundEngine::playSoundBufferUpdate_( SoundSourceDesc * _source )
    {
        _source->taskSoundBufferUpdate = 
            m_poolWorkerTaskSoundBufferUpdate.createObjectT();

        SoundBufferInterface * soundBuffer = _source->soundSourceInterface->getSoundBuffer();

        _source->taskSoundBufferUpdate->initialize( m_serviceProvider, soundBuffer );

        _source->taskSoundBufferUpdateId = m_threadSoundBufferUpdate.addWorker( _source->taskSoundBufferUpdate );
    }
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getPosMs( unsigned int _emitter ) const
	{
        const SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:getPosMs not found emitter id %d"
				, _emitter
				);

			return 0.f;
		}

		if( source->soundSourceInterface == 0 )
		{
			return 0.f;
		}

        float pos = source->soundSourceInterface->getPosMs();
		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
}
