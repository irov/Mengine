#	include "SoundEngine.h"

#	include "LogEngine.h"
#	include "Interface/SoundCodecInterface.h"

#	include "TaskManager.h"
#	include "TaskSoundBufferUpdate.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundSulkCallback::~SoundSulkCallback()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::SoundEngine()
	: m_interface( NULL )
	, m_sulkcallback(0)
	, m_soundVolume(1.0f)
	, m_commonVolume(1.0f)
	, m_musicVolume(1.0f)
	, m_initialized(false)
	, m_muted(false)
	, m_enumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::~SoundEngine()
	{
		if( m_interface != NULL )
		{
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::initialize()
	{
		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false ) || ( m_interface == NULL ) )
		{
			return false;
		}

		if( m_interface != 0 )
		{
			LogSystemInterface * logInterface = LogEngine::get()->getInterface();

			m_initialized = m_interface->initialize( logInterface );
		}
		return m_initialized;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setListenerOrient( const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top )
	{
		m_interface->setListenerOrient( _position.buff(), _front.buff(), top.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int SoundEngine::createSoundSource(
			bool _isHeadMode, 
			SoundBufferInterface * _sample,
			bool _music /* = false */ )
	{
		SoundSourceInterface* sourceInterface = 
			m_interface->createSoundSource( _isHeadMode, _sample );

		if( sourceInterface == 0 )
		{
			MENGE_LOG_ERROR("SoundEngine: create SoundSource invalid");

			return 0;
		}

		++m_enumerator;
		unsigned int soundId = m_enumerator;

		TSoundSource source;
		source.soundSourceInterface = sourceInterface;
		source.listener = NULL;
		source.taskSoundBufferUpdate = NULL;
		source.timing = 0.f;
		source.volume = 1.f;
		source.state = Stopped;
		source.music = _music;
		source.looped = false;

		m_soundSourceMap.insert( std::make_pair( soundId, source ) );

		if( _music )
		{
			sourceInterface->setVolume( m_commonVolume * m_musicVolume );
		}
		else
		{
			sourceInterface->setVolume( m_commonVolume * m_soundVolume );
		}

		if( m_muted == true )
		{
			sourceInterface->setVolume( 0.0f );
		}

		return	soundId;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface * SoundEngine::createSoundBufferFromFile( const ConstString& _pakName, const ConstString & _filename, const ConstString & _codecType, bool _isStream )
	{
		SoundDesc desc;

		desc.stream = FileEngine::get()
			->openInputFile( _pakName, Helper::to_str(_filename) );

		desc.codec = CodecEngine::get()
			->createDecoderT<SoundDecoderInterface>( _codecType, desc.stream );
		
    	if( desc.codec == NULL )
		{
			MENGE_LOG_ERROR( "Error: Can't create sound decoder for file '%s'"
				, _filename.c_str() 
				);

			desc.stream->close();

			return NULL;
		}

		//this need for ffmpeg to ogg convertion
		SoundCodecOptions opt;
		String inputFileFullPath;
		FileEngine::get()->getFullPath( _pakName, _filename, inputFileFullPath );
		opt.inputFileFullPath = inputFileFullPath;
		desc.codec->setOptions(&opt);
		//end

		SoundBufferInterface* sample = 
			m_interface->createSoundBuffer( desc.codec, _isStream );

		if( _isStream  == true )
		{
			m_bufferStreams.insert( std::make_pair( sample, desc ) );
		}
		else
		{
			desc.codec->destroy();

			desc.stream->close();
		}

		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface *	SoundEngine::createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem )
	{
		//SoundBufferInterface * sample =  
		//	m_interface->createSoundBufferFromMemory( _buffer, _size, _newmem );
		assert( 0 && "Not Implemented yet" );
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundBuffer( SoundBufferInterface* _soundBuffer )
	{
		m_interface->releaseSoundBuffer( _soundBuffer );

		TMapBufferStreams::iterator it_find = m_bufferStreams.find( _soundBuffer );
				
		if( it_find == m_bufferStreams.end() )
		{
			return;
		}

		SoundDesc & desc = it_find->second;

		desc.codec->destroy();

		desc.stream->close();

		m_bufferStreams.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundSource( unsigned int _sourceID )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _sourceID );
		
		if( it_find == m_soundSourceMap.end() )
		{
			return;
		}

		if( m_interface != NULL )
		{
			if( it_find->second.taskSoundBufferUpdate != NULL )
			{
				it_find->second.taskSoundBufferUpdate->stop();
				
				TaskManager::get()
					->joinTask( it_find->second.taskSoundBufferUpdate );
			}

			it_find->second.soundSourceInterface->stop();
			m_interface->releaseSoundNode( it_find->second.soundSourceInterface );
		}

		m_soundSourceMap.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSoundVolume( float _volume )
	{
		m_soundVolume = _volume;
		updateVolume_();
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
		updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getCommonVolume() const
	{
		return m_commonVolume;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::setBlow( bool _active )
	{
		return m_interface->setBlow( _active );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getBlow()
	{
		return m_interface->getBlow();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setEnoughBlow( float _enoughBlow )
	{
		m_interface->setEnoughBlow( _enoughBlow );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::update( float _timing )
	{
		for( TSoundSourceMap::iterator 
			it = m_soundSourceMap.begin(), 
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
			TSoundSource& source = it->second;
			switch( source.state )
			{
			case Stopped:
			case Paused:
				break;
			case StopPlay:
			case PausePlay:
				if( source.taskSoundBufferUpdate != NULL )
				{
					break; // can't start playing
				}
				source.state = Playing;
				source.timing = source.soundSourceInterface->getLengthMs();
				source.soundSourceInterface->play();
				if( source.music == true )
				{
					source.taskSoundBufferUpdate = new TaskSoundBufferUpdate( &source );
					TaskManager::get()
						->addTask( source.taskSoundBufferUpdate );
				}
				break;
			case Playing:
				if( source.looped == false )
				{
					source.timing -= _timing;
					if( source.timing <= 0.0f )
					{
						if( source.music == true &&
							source.taskSoundBufferUpdate != NULL )
						{
							source.state = Stopping;
							source.taskSoundBufferUpdate->stop();
						}
						else
						{
							source.state = Stopped;
							source.soundSourceInterface->stop();
						}
						if( source.listener != NULL )
						{
							m_stopListeners.push_back( source.listener );
						}
					}
				}
				break;
			case Stopping:
				if( source.music == true &&
					source.taskSoundBufferUpdate != NULL )
				{
					source.taskSoundBufferUpdate->stop();
				}
				else
				{
					source.state = Stopped;
					source.soundSourceInterface->stop();
				}
				if( source.listener != NULL )
				{
					m_stopListeners.push_back( source.listener );
				}
				break;
			case Pausing:
				if( source.music == true &&
					source.taskSoundBufferUpdate != NULL )
				{
					source.state = Pausing;
					source.taskSoundBufferUpdate->stop();
				}
				else
				{
					source.state = Paused;
					source.soundSourceInterface->pause();
				}
				if( source.listener != NULL )
				{
					m_pauseListeners.push_back( source.listener );
				}
				break;
			case NeedRestart:
				if( source.music == true &&
					source.taskSoundBufferUpdate != NULL )
				{
					break;	// can't restart until stopped
				}
				source.state = Playing;
				source.timing = source.soundSourceInterface->getLengthMs();
				source.soundSourceInterface->stop();
				source.soundSourceInterface->play();
				if( source.music == true &&
					source.taskSoundBufferUpdate != NULL )
				{
					source.taskSoundBufferUpdate = new TaskSoundBufferUpdate( &source );
					TaskManager::get()
						->addTask( source.taskSoundBufferUpdate );
				}
				if( source.listener != NULL )
				{
					m_stopListeners.push_back( source.listener );
				}
				break;
			}
		}

		// listeners should not be changed here
		// e.g. setSourceListener( ... ) should not be called before this moment
		for( TSourceListenerVector::iterator 
			it = m_stopListeners.begin(), 
			it_end = m_stopListeners.end();
		it != it_end;
		++it )
		{
			(*it)->listenStopped();
		}
	
		m_stopListeners.clear();

		for( TSourceListenerVector::iterator 
			it = m_pauseListeners.begin(), 
			it_end = m_pauseListeners.end();
		it != it_end;
		++it )
		{
			(*it)->listenPaused();
		}
	
		m_pauseListeners.clear();

		m_interface->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSulkCallback( SoundSulkCallback * _sulkcallback )
	{
		delete m_sulkcallback;
		m_sulkcallback = _sulkcallback;
		m_interface->setBlowCallback( m_sulkcallback );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::mute( bool _mute )
	{
		m_muted = _mute;

		updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::isMute() const
	{
		return m_muted;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::onFocus( bool _focus )
	{
		if( _focus == false )
		{
			for( TSoundSourceMap::iterator 
				it = m_soundSourceMap.begin(),	
				it_end = m_soundSourceMap.end();
			it != it_end;
			++it )
			{
				if( it->second.state == Playing )
				{
					if( it->second.taskSoundBufferUpdate != NULL )
					{
						it->second.taskSoundBufferUpdate->stop();

						TaskManager::get()
							->joinTask( it->second.taskSoundBufferUpdate );
					}
					it->second.soundSourceInterface->pause();
				}
				else if( it->second.state == Stopping
					&& it->second.taskSoundBufferUpdate != NULL )
				{
					TaskManager::get()
						->joinTask( it->second.taskSoundBufferUpdate );
				}
			}
		}
		else
		{
			for( TSoundSourceMap::iterator 
				it = m_soundSourceMap.begin(), 
				it_end = m_soundSourceMap.end();
			it != it_end;
			++it )
			{
				if( it->second.state == Playing )
				{
					it->second.soundSourceInterface->play();
					if( it->second.music == true && it->second.taskSoundBufferUpdate == NULL )
					{
						it->second.taskSoundBufferUpdate = new TaskSoundBufferUpdate( &(it->second) );
						TaskManager::get()
							->addTask( it->second.taskSoundBufferUpdate );
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::play( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:play not found emitter id %d"
				, _emitter
				);

			return;
		}

		TSoundSource& source = it_find->second;
		switch( source.state )
		{
		case Stopped:
			source.state = StopPlay;
			break;
		case Paused:
			source.state = PausePlay;
			break;
		case Stopping:
			source.state = NeedRestart;
			break;
		case Pausing:
			source.state = Playing;
			break;
		case StopPlay:
		case PausePlay:
		case Playing:
		case NeedRestart:
			// nothing to do
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::pause( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:pause not found emitter id %d"
				, _emitter
				);

			return;
		}

		TSoundSource& source = it_find->second;
		switch( source.state )
		{
		case Stopped:
		case Paused:
		case Stopping:
		case Pausing:
			// nothing to do
			break;
		case StopPlay:
			source.state = Stopped;
			break;
		case PausePlay:
			source.state = Paused;
			break;
		case Playing:
			source.state = Pausing;
			break;
		case NeedRestart:
			source.state = Stopping;
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::stop( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:stop not found emitter id %d"
				, _emitter
				);

			return;
		}

		TSoundSource& source = it_find->second;

		switch( source.state )
		{
		case Stopped:
		case Stopping:
			// nothing to do
			break;
		case StopPlay:
			source.state = Stopped;
			break;
		case Playing:
		case NeedRestart:
		case Paused:
		case Pausing:
		case PausePlay:
			source.state = Stopping;
			break;
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setLoop( unsigned int _emitter, bool _looped )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:setLoop not found emitter id %d"
				, _emitter
				);

			return;
		}

		TSoundSource& source = it_find->second;
		source.looped = _looped;
		source.soundSourceInterface->setLoop( _looped );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSourceListener( unsigned int _emitter, SoundNodeListenerInterface* _listener )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:setSourceListener not found emitter id %d"
				, _emitter
				);

			return;
		}
	
		it_find->second.listener = _listener;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::updateVolume_()
	{
		for( TSoundSourceMap::iterator 
			it = m_soundSourceMap.begin(), 
			it_end = m_soundSourceMap.end();
		it != it_end;
		++it )
		{
			if( m_muted == true )
			{
				it->second.soundSourceInterface->setVolume( 0.0f );
			}
			else
			{
				float volume = it->second.volume;
				if( it->second.music == false )
				{
					it->second.soundSourceInterface->setVolume( m_commonVolume * m_soundVolume * volume );
				}
				else
				{
					it->second.soundSourceInterface->setVolume( m_commonVolume * m_musicVolume * volume );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSourceVolume( unsigned int _emitter, float _volume )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:setVolume not found emitter id %d"
				, _emitter
				);

			return;
		}

		it_find->second.volume = _volume;
		if( m_muted == false )
		{
			it_find->second.soundSourceInterface->setVolume( m_commonVolume * m_soundVolume * _volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::getLoop( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:getLoop not found emitter id %d"
				, _emitter
				);

			return false;
		}

		return it_find->second.looped;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSourceVolume( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:getVolume not found emitter id %d"
				, _emitter
				);

			return 0.0f;
		}

		return it_find->second.volume;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getLengthMs( unsigned int _emitter ) const
	{
		TSoundSourceMap::const_iterator it_find = m_soundSourceMap.find( _emitter );

		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:getLengthMs not found emitter id %d"
				, _emitter
				);

			return 0.0f;
		}
		
		return it_find->second.soundSourceInterface->getLengthMs();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setMusicVolume( float _volume )
	{
		m_musicVolume = _volume;
		updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getMusicVolume() const
	{
		return m_musicVolume;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setPosMs( unsigned int _emitter, float _pos )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:setPosMs not found emitter id %d"
				, _emitter
				);

			return;
		}

		it_find->second.soundSourceInterface->setPosMs( _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getPosMs( unsigned int _emitter ) const
	{
		TSoundSourceMap::const_iterator it_find = m_soundSourceMap.find( _emitter );

		if( it_find == m_soundSourceMap.end() )
		{
			MENGE_LOG_ERROR("SoundEngine:getPosMs not found emitter id %d"
				, _emitter
				);

			return 0.0f;
		}
		
		return it_find->second.soundSourceInterface->getPosMs();
	}
	//////////////////////////////////////////////////////////////////////////
}
