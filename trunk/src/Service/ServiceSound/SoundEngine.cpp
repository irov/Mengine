#	include "SoundEngine.h"

#	include "Interface/SoundCodecInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"

#	include "ThreadTaskSoundBufferUpdate.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundService, Menge::SoundServiceInterface, Menge::SoundEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::SoundEngine()
		: m_soundSystem(NULL)
        , m_serviceProvider(NULL)
		, m_soundVolume(1.f)
		, m_commonVolume(1.f)
		, m_musicVolume(1.f)
        , m_voiceVolume(1.f)
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

        m_soundSystem = SOUND_SYSTEM(m_serviceProvider);
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
            SoundSourceDesc & desc = it->second;

            if( desc.taskSoundBufferUpdate != NULL )
            {
                desc.taskSoundBufferUpdate->stop();

                THREAD_SERVICE(m_serviceProvider)->joinTask( desc.taskSoundBufferUpdate );

                delete desc.taskSoundBufferUpdate;
                desc.taskSoundBufferUpdate = NULL;
            }

            desc.soundSourceInterface->stop();

            if( m_soundSystem != NULL )
            {
                m_soundSystem->releaseSoundNode( desc.soundSourceInterface );
            }
        }

		if( m_soundSystem != NULL )
		{
            m_soundSystem->finalize();
			m_soundSystem = NULL;
		}
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
            SoundSourceDesc & desc = it->second;

			if( desc.state == ESS_PLAYING )
			{
				desc.soundSourceInterface->play();

				if( desc.streamable == true && desc.taskSoundBufferUpdate == NULL )
				{
					desc.taskSoundBufferUpdate = new ThreadTaskSoundBufferUpdate(m_serviceProvider, &desc);

					THREAD_SERVICE(m_serviceProvider)
                        ->addTask( desc.taskSoundBufferUpdate );
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
            SoundSourceDesc & desc = it->second;

			if( desc.state == ESS_PLAYING )
			{
				if( desc.taskSoundBufferUpdate != NULL )
				{
					desc.taskSoundBufferUpdate->stop();

					THREAD_SERVICE(m_serviceProvider)
                        ->joinTask( desc.taskSoundBufferUpdate );

                    delete desc.taskSoundBufferUpdate;
                    desc.taskSoundBufferUpdate = NULL;
				}

				desc.soundSourceInterface->pause();
			}
			else if( desc.state == ESS_STOPPING )
			{
				if( desc.taskSoundBufferUpdate != NULL )
				{
					desc.taskSoundBufferUpdate->stop();

					THREAD_SERVICE(m_serviceProvider)
                        ->joinTask( desc.taskSoundBufferUpdate );

                    delete desc.taskSoundBufferUpdate;
                    desc.taskSoundBufferUpdate = NULL;
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
			this->stopSounds_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
    void SoundEngine::onTurnSound( bool _turn )
    {
		m_turn = _turn;
        
		m_soundSystem->onTurnSound( m_turn );

		this->updateVolume_();
    }
	//////////////////////////////////////////////////////////////////////////
	unsigned int SoundEngine::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, ESoundSourceType _type, bool _streamable )
	{
		SoundSourceInterface* sourceInterface = 
			m_soundSystem->createSoundSource( _isHeadMode, _sample );

		if( sourceInterface == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::createSoundSource: create SoundSource invalid"
                );

			return 0;
		}

		++m_enumerator;
		unsigned int soundId = m_enumerator;

		SoundSourceDesc source;
		source.soundSourceInterface = sourceInterface;
		source.listener = NULL;
		source.taskSoundBufferUpdate = NULL;
		source.timing = 0.f;
		source.volume = 1.f;
		source.state = ESS_STROPPED;        
		source.type = _type;
        source.streamable = _streamable;
		source.looped = false;

		TMapSoundSource::iterator it_insert = 
            m_soundSourceMap.insert( std::make_pair( soundId, source ) ).first;
        
        SoundSourceDesc * insert_source = &it_insert->second;

        this->updateSourceVolume_( insert_source, 1.f );

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
                    sourceInterface->setVolume( m_commonVolume * m_soundVolume * _volume );
                }break;
            case EST_MUSIC:
                {
                    sourceInterface->setVolume( m_commonVolume * m_musicVolume * _volume );
                }break;
            case EST_VOICE:
                {
                    sourceInterface->setVolume( m_commonVolume * m_voiceVolume * _volume );
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

        if( desc.stream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundEngine::createSoundBufferFromFile: Can't open sound file %s:%s"
                , _pakName.c_str()
                , _filename.c_str() 
                );

            return NULL;
        }

		desc.codec = CODEC_SERVICE(m_serviceProvider)->createDecoderT<SoundDecoderInterface>( _codecType, desc.stream );
		
    	if( desc.codec == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)( "SoundEngine::createSoundBufferFromFile: Can't create sound decoder for file %s:%s"
                , _pakName.c_str()
				, _filename.c_str() 
				);

			desc.stream->destroy();

			return NULL;
		}

		SoundBufferInterface* sample = 
			m_soundSystem->createSoundBuffer( desc.codec, _isStream );

        if( sample == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundEngine::createSoundBufferFromFile: Can't create sound buffer for file %s:%s"
                , _pakName.c_str()
                , _filename.c_str() 
                );

            desc.codec->destroy();
            desc.stream->destroy();

            return NULL;
        }

		if( _isStream == true )
		{
			m_bufferStreams.insert( std::make_pair( sample, desc ) );
		}
		else
		{
			desc.codec->destroy();
			desc.stream->destroy();
		}

		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundBuffer( SoundBufferInterface* _soundBuffer )
	{
		m_soundSystem->releaseSoundBuffer( _soundBuffer );

		TMapBufferStreams::iterator it_find = m_bufferStreams.find( _soundBuffer );
				
		if( it_find == m_bufferStreams.end() )
		{
			return;
		}

		SoundDesc & desc = it_find->second;

		desc.codec->destroy();

		desc.stream->destroy();

		m_bufferStreams.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundSource( unsigned int _sourceID )
	{
		TMapSoundSource::iterator it_find = m_soundSourceMap.find( _sourceID );
		
		if( it_find == m_soundSourceMap.end() )
		{
			return;
		}

        SoundSourceDesc & desc = it_find->second;

        if( desc.taskSoundBufferUpdate != NULL )
        {
            desc.taskSoundBufferUpdate->stop();

            THREAD_SERVICE(m_serviceProvider)
                ->joinTask( desc.taskSoundBufferUpdate );

            delete desc.taskSoundBufferUpdate;
            desc.taskSoundBufferUpdate = NULL;
        }

        desc.soundSourceInterface->stop();

        m_soundSystem->releaseSoundNode( desc.soundSourceInterface );

		m_soundSourceMap.erase( it_find );
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
	void SoundEngine::setSoundVolume( float _volume )
	{
		m_soundVolume = _volume;

		this->updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSoundVolume() const
	{
		return m_soundVolume;
	}
	//////////////////////////////////////////////////////////////////////////	
	void SoundEngine::setCommonVolume( float _volume )
	{
		m_commonVolume = _volume;

		this->updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getCommonVolume() const
	{
		return m_commonVolume;
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
			SoundSourceDesc & source = it->second;

			switch( source.state )
			{
			case ESS_STROPPED:
			case ESS_PAUSED:
                {
                }break;
			case ESS_STOP_PLAY:
			case ESS_PAUSE_PLAY:
                {
                }break;
            case ESS_PLAYING:
                {
                    if( source.looped == false )
                    {
                        source.timing -= _timing;

                        if( source.timing <= 0.f )
                        {
                            if( source.streamable == true && source.taskSoundBufferUpdate != NULL )
                            {
                                source.state = ESS_STOPPING;
                                source.taskSoundBufferUpdate->stop();
                            }
                            else
                            {
                                source.state = ESS_STROPPED;
                                source.soundSourceInterface->stop();
                            }

                            if( source.listener != NULL )
                            {
                                m_stopListeners.push_back( source.listener );
                            }
                        }
                    }
                }break;
			case ESS_STOPPING:
                {
                    if( source.streamable == true && source.taskSoundBufferUpdate != NULL )
                    {
                        source.taskSoundBufferUpdate->stop();
                    }
                    else
                    {
                        source.state = ESS_STROPPED;
                        source.soundSourceInterface->stop();
                    }

                    if( source.listener != NULL )
                    {
                        m_stopListeners.push_back( source.listener );
                    }
                }break;
            case ESS_PAUSING:
                {
                    if( source.streamable == true && source.taskSoundBufferUpdate != NULL )
                    {
                        source.taskSoundBufferUpdate->stop();
                    }
                    else
                    {
                        source.state = ESS_PAUSED;
                        source.soundSourceInterface->pause();
                    }

                    if( source.listener != NULL )
                    {
                        m_pauseListeners.push_back( source.listener );
                    }
                }break;
            case ESS_NEED_RESTART:
                {
                    //if( source.streamable == true && source.taskSoundBufferUpdate != NULL )
                    //{
                    //    break;	// can't restart until stopped
                    //}

                    if( source.taskSoundBufferUpdate != NULL )
                    {
                        source.taskSoundBufferUpdate->stop();

                        THREAD_SERVICE(m_serviceProvider)
                            ->joinTask( source.taskSoundBufferUpdate );

                        delete source.taskSoundBufferUpdate;
                        source.taskSoundBufferUpdate = NULL;
                    }

                    source.state = ESS_PLAYING;
                    source.timing = source.soundSourceInterface->getLengthMs();
                    source.soundSourceInterface->stop();

                    source.soundSourceInterface->play();

                    if( source.streamable == true )
                    {
                        source.taskSoundBufferUpdate = new ThreadTaskSoundBufferUpdate(m_serviceProvider, &source);

                        THREAD_SERVICE(m_serviceProvider)
                            ->addTask( source.taskSoundBufferUpdate );
                    }

                    if( source.listener != NULL )
                    {
                        m_stopListeners.push_back( source.listener );
                    }
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
        TMapSoundSource::iterator it_find = m_soundSourceMap.find( _emitterId );

        if( it_find == m_soundSourceMap.end() )
        {
            return false;
        }

        *_desc = &it_find->second;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundEngine::getSoundSourceDesc_( unsigned int _emitterId, const SoundSourceDesc ** _desc ) const
    {
        TMapSoundSource::const_iterator it_find = m_soundSourceMap.find( _emitterId );

        if( it_find == m_soundSourceMap.end() )
        {
            return false;
        }

        *_desc = &it_find->second;

        return true;
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
		case ESS_STROPPED:
            {
                if( source->taskSoundBufferUpdate != NULL )
                {
                    source->taskSoundBufferUpdate->stop();

                    THREAD_SERVICE(m_serviceProvider)
                        ->joinTask( source->taskSoundBufferUpdate );

                    delete source->taskSoundBufferUpdate;
                    source->taskSoundBufferUpdate = NULL;
                }

                source->timing = source->soundSourceInterface->getLengthMs();
                source->soundSourceInterface->play();

                source->state = ESS_PLAYING;

                if( source->streamable == true )				
                {
                    source->taskSoundBufferUpdate = new ThreadTaskSoundBufferUpdate(m_serviceProvider, source);

                    THREAD_SERVICE(m_serviceProvider)
                        ->addTask( source->taskSoundBufferUpdate );
                }
            }break;
		case ESS_PAUSED:
			source->state = ESS_PAUSE_PLAY;
			break;
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
		case ESS_STROPPED:
		case ESS_PAUSED:
		case ESS_STOPPING:
		case ESS_PAUSING:
			// nothing to do
			break;
		case ESS_STOP_PLAY:
			source->state = ESS_STROPPED;
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
		case ESS_STROPPED:
		case ESS_STOPPING:
			// nothing to do
			break;
		case ESS_STOP_PLAY:
			source->state = ESS_STROPPED;
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
            SoundSourceDesc * source = &it->second;

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
	void SoundEngine::setMusicVolume( float _volume )
	{
		m_musicVolume = _volume;

		this->updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getMusicVolume() const
	{
		return m_musicVolume;
	}
    //////////////////////////////////////////////////////////////////////////
    void SoundEngine::setVoiceVolume( float _volume )
    {
        m_voiceVolume = _volume;

        this->updateVolume_();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundEngine::getVoiceVolume() const
    {
        return m_voiceVolume;
    }
    //////////////////////////////////////////////////////////////////////////
	void SoundEngine::setPosMs( unsigned int _emitter, float _pos )
	{
        SoundSourceDesc * source;
        if( this->getSoundSourceDesc_( _emitter, &source ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("SoundEngine:setPosMs not found emitter id %d"
				, _emitter
				);

			return;
		}

        if( source->soundSourceInterface == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine:setPosMs not setup source %d"
                , _emitter
                );

            return;
        }

        float current_pos = source->soundSourceInterface->getPosMs();

        if( fabsf( current_pos - _pos ) < 0.0001f )
        {
            return;
        }

        if( source->state == ESS_STROPPED || source->state == ESS_STOPPING )
        {
            return;
        }
        
        bool hasBufferUpdate = source->taskSoundBufferUpdate != NULL;
        
        if( hasBufferUpdate == true )
        {
            source->taskSoundBufferUpdate->stop();

            THREAD_SERVICE(m_serviceProvider)
                ->joinTask( source->taskSoundBufferUpdate );

            delete source->taskSoundBufferUpdate;
            source->taskSoundBufferUpdate = NULL;
        }

        source->soundSourceInterface->pause();
        
		source->soundSourceInterface->setPosMs( _pos );

        source->soundSourceInterface->play();

        if( hasBufferUpdate == true )				
        {
            source->taskSoundBufferUpdate = new ThreadTaskSoundBufferUpdate(m_serviceProvider, source);

            THREAD_SERVICE(m_serviceProvider)
                ->addTask( source->taskSoundBufferUpdate );
        }
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
