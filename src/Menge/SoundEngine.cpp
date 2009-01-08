#	include "SoundEngine.h"

#	include "LogEngine.h"
#	include "Codec.h"
#	include "Interface/SoundCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundSulkCallback::~SoundSulkCallback()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::SoundEngine( SoundSystemInterface * _interface )
	: m_interface( _interface )
	, m_sulkcallback(0)
	, m_soundVolume( 1.0f )
	, m_commonVolume( 1.0f )
	, m_musicVolume( 1.0f )
	, m_initialized( false )
	, m_muted( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::~SoundEngine()
	{
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
		SoundSourceInterface* sourceInterface = m_interface->createSoundSource( _isHeadMode, _sample );
		if( sourceInterface == 0 )
		{
			return 0;
		}
		static unsigned int count = 0;
		count++;
		
		TSoundSource source( sourceInterface, Stopped, NULL, 0.0f, false, 1.0f, _music );
		m_soundSourceMap.insert( std::make_pair( count, source ) );
		if( m_muted == true )
		{
			sourceInterface->setVolume( 0.0f );
		}

		return	count;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface * SoundEngine::createSoundBufferFromFile( const String & _filename, bool _isStream )
	{
		SoundDecoderInterface* soundDecoder = DecoderManager::createDecoderT<SoundDecoderInterface>( _filename, "Sound" );
		if( soundDecoder == NULL )
		{
			MENGE_LOG_ERROR( "Error: Can't create sound decoder for file \"%s\"",
						_filename.c_str() );
			return NULL;
		}

		SoundBufferInterface* sample = 
			m_interface->createSoundBuffer( soundDecoder, _isStream );

		if( _isStream  == true )
		{
			m_bufferStreams.insert( std::make_pair( sample, soundDecoder ) );
		}
		else
		{
			DecoderManager::releaseDecoder( soundDecoder );
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
		TMapBufferStreams::iterator it_find = m_bufferStreams.find( _soundBuffer );
		SoundDecoderInterface* soundDecoder = NULL;
		if( it_find != m_bufferStreams.end() )
		{
			soundDecoder = it_find->second;
			m_bufferStreams.erase( it_find );
		}
		m_interface->releaseSoundBuffer( _soundBuffer );
		if( soundDecoder != NULL )
		{
			DecoderManager::releaseDecoder( soundDecoder );
		}
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
			it_find->second.soundSourceInterface->stop();
			m_interface->releaseSoundNode( it_find->second.soundSourceInterface );
		}

		m_soundSourceMap.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSoundSourceVolume( float _volume )
	{
		m_soundVolume = _volume;
		updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSoundSourceVolume() const
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
		for( TSoundSourceMap::iterator it = m_soundSourceMap.begin(), it_end = m_soundSourceMap.end();
			it != it_end;
			it++ )
		{
			TSoundSource& source = it->second;
			switch( source.state )
			{
			case Stopped:
			case Paused:
				break;
			case StopPlay:
			case PausePlay:
				source.state = Playing;
				source.timing = source.soundSourceInterface->getLengthMs();
				source.soundSourceInterface->play();
				break;
			case Playing:
				if( source.looped == false )
				{
					source.timing -= _timing;
					if( source.timing <= 0.0f )
					{
						source.state = Stopped;
						source.soundSourceInterface->stop();
						if( source.listener != NULL )
						{
							m_stopListeners.push_back( source.listener );
						}
					}
				}
				break;
			case Stopping:
				source.state = Stopped;
				source.soundSourceInterface->stop();
				if( source.listener != NULL )
				{
					m_stopListeners.push_back( source.listener );
				}
				break;
			case Pausing:
				source.state = Paused;
				source.soundSourceInterface->pause();
				if( source.listener != NULL )
				{
					m_pauseListeners.push_back( source.listener );
				}
				break;
			case NeedRestart:
				source.state = Playing;
				source.soundSourceInterface->stop();
				source.soundSourceInterface->play();
				if( source.listener != NULL )
				{
					m_stopListeners.push_back( source.listener );
				}
				break;
			}
		}

		// listeners should not be changed here
		// e.g. setSourceListener( ... ) should not be called before this moment
		for( TSourceListenerVector::iterator it = m_stopListeners.begin(), it_end = m_stopListeners.end();
			it != it_end;
			it++ )
		{
			(*it)->listenStopped();
		}
		m_stopListeners.clear();

		for( TSourceListenerVector::iterator it = m_pauseListeners.begin(), it_end = m_pauseListeners.end();
			it != it_end;
			it++ )
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
	bool SoundEngine::initialize()
	{
		if( m_interface != 0 )
		{
			m_initialized = m_interface->initialize( Holder<LogEngine>::hostage()->getInterface() );
		}
		return m_initialized;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::mute( bool _mute )
	{
		m_muted = _mute;
		updateVolume_();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::onFocus( bool _focus )
	{
		if( _focus == false )
		{
			for( TSoundSourceMap::iterator it = m_soundSourceMap.begin(), it_end = m_soundSourceMap.end();
				it != it_end;
				it++ )
			{
				if( it->second.state == Playing )
				{
					it->second.soundSourceInterface->pause();
				}
			}
		}
		else
		{
			for( TSoundSourceMap::iterator it = m_soundSourceMap.begin(), it_end = m_soundSourceMap.end();
				it != it_end;
				it++ )
			{
				if( it->second.state == Playing )
				{
					it->second.soundSourceInterface->play();
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
	void SoundEngine::setLooped( unsigned int _emitter, bool _looped )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			return;
		}

		TSoundSource& source = it_find->second;
		source.looped = _looped;
		source.soundSourceInterface->setLooped( _looped );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSourceListener( unsigned int _emitter, SoundNodeListenerInterface* _listener )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			return;
		}
	
		it_find->second.listener = _listener;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::updateVolume_()
	{
		for( TSoundSourceMap::iterator it = m_soundSourceMap.begin(), it_end = m_soundSourceMap.end();
			it != it_end;
			it++ )
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
	void SoundEngine::setVolume( unsigned int _emitter, float _volume )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			return;
		}

		it_find->second.volume = _volume;
		if( m_muted == false )
		{
			it_find->second.soundSourceInterface->setVolume( m_commonVolume * m_soundVolume * _volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::isLooped( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			return false;
		}

		return it_find->second.looped;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getVolume( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			return 0.0f;
		}

		return it_find->second.volume;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getLengthMs( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
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
			return;
		}

		it_find->second.soundSourceInterface->setPosMs( _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getPosMs( unsigned int _emitter )
	{
		TSoundSourceMap::iterator it_find = m_soundSourceMap.find( _emitter );
		if( it_find == m_soundSourceMap.end() )
		{
			return 0.0f;
		}
		
		return it_find->second.soundSourceInterface->getPosMs();
	}
	//////////////////////////////////////////////////////////////////////////
}