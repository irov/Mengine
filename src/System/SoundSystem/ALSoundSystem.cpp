
#	include "ALSoundBuffer.h"
#	include "ALSoundBufferStream.h"
#	include "ALSoundSource.h"
#	include "ALSoundSystem.h"

#	include "SulkSystem.h"

#include <algorithm>

#	include "Menge/LogEngine.h"

//////////////////////////////////////////////////////////////////////////
ALSoundSystem::ALSoundSystem()
: m_device(0) 
, m_soundVelocity(343.0f)
, m_dopplerFactor(1.0f)
, m_distanceModel(None)
, m_initialized( false )
, m_alSourcesNum( 0 )
{
}
//////////////////////////////////////////////////////////////////////////
ALSoundSystem::~ALSoundSystem()
{
	if( m_initialized )
	{
		for( TSourceMap::iterator it = m_sources.begin(), it_end = m_sources.end();
			it != it_end;
			it++ )
		{
			delete it->first;
		}
		m_sources.clear();

		for( TALSourceMap::iterator it = m_alSourcesStereo.begin(), it_end = m_alSourcesStereo.end();
			it != it_end;
			it++ )
		{
			alDeleteSources( 1, &(it->first) );
		}

		for( TALSourceMap::iterator it = m_alSourcesMono.begin(), it_end = m_alSourcesMono.end();
			it != it_end;
			it++ )
		{
			alDeleteSources( 1, &(it->first) );
		}

		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_context);
		alcCloseDevice(m_device);

		delete m_sulk;
	}
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundSystem::initialize( Menge::LogSystemInterface* _logSystem )
{
	if( m_initialized )
	{
		return false;
	}

	m_logSystem = _logSystem;
	MENGE_LOG( "Initializing OpenAL sound system..." );

	//char *initString = 0L;//"DirectSound", "DirectSound3D", ;
	m_device = alcOpenDevice( NULL );

	if( !m_device )
	{
		MENGE_LOG_WARNING( "OpenAL failed to open device. Sound will be disabled" );
		return false;
	}

	m_context = alcCreateContext( m_device, NULL );
	if( !m_context || alcGetError( m_device ) != ALC_NO_ERROR) 
	{
		if( m_context )
		{
			alcDestroyContext( m_context );
		}
		alcCloseDevice( m_device );
		MENGE_LOG_WARNING( "OpenAL failed to create context. Sound will be disabled" );
		return false;
	} 

	alcMakeContextCurrent( m_context );

	//
	m_soundVelocity = 343.0f;
	m_dopplerFactor = 1.0f;
	m_distanceModel = None;

	for( int i = 0; i < MAX_SOURCENAMES_NUM; i++ )
	{
		ALuint sourceName;
		alGenSources(1, &sourceName);	
		if(alGetError())
			break;
		if( i % 2 == 0 )
		{
			m_alSourcesStereo.insert( std::make_pair( sourceName, false ) );
		}
		else
		{
			m_alSourcesMono.insert( std::make_pair( sourceName, false ) );
		}
	}
	m_alSourcesNum = m_alSourcesMono.size() + m_alSourcesStereo.size();

	// Environment settings 
	alSpeedOfSound(m_soundVelocity);
	alDopplerFactor(m_dopplerFactor);
	setDistanceModel(m_distanceModel);

	m_sulk = new SulkSystem();

	m_initialized = true;
	MENGE_LOG( "OpenAL sound system initialized successfully" );
	return true;
}
//////////////////////////////////////////////////////////////////////////
void    ALSoundSystem::setListenerOrient( float * _position, float * _front, float * _top)
{
	if( m_initialized == false ) return;

	memcpy( m_listenerPosition, _position, sizeof(float)*3 );
	memcpy( m_listenerOrientation, _front, sizeof(float)*3 );
	memcpy( m_listenerOrientation+3, _top, sizeof(float)*3 );

	alListenerfv(AL_POSITION, m_listenerPosition);
	alListenerfv(AL_ORIENTATION, m_listenerOrientation);
}
//////////////////////////////////////////////////////////////////////////
Menge::SoundSourceInterface * ALSoundSystem::createSoundSource( bool _isHeadMode, Menge::SoundBufferInterface * _sample, Menge::SoundNodeListenerInterface * _listener )
{
	if( m_initialized == false ) return 0;
	
	ALSoundSource* source = 0;

	for( TSourceMap::iterator it = m_sources.begin(), it_end = m_sources.end();
		it != it_end;
		it++ )
	{
		if( it->second == false )
		{
			source = it->first;
			it->second = true;
			break;
		}
	}

	if( !source )
	{
		source = new ALSoundSource(this);
		m_sources.insert( std::make_pair( source, true ) );
	}

	source->setSoundBuffer( static_cast<ALSoundBuffer*>(_sample) );
	source->setAmbient( _isHeadMode );
	source->setSoundNodeListener( _listener );

	return source;
}
//////////////////////////////////////////////////////////////////////////
Menge::SoundBufferInterface *  ALSoundSystem::createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem )
{
	if( m_initialized == false ) return 0;

	ALSoundBuffer* buffer = new ALSoundBuffer();
	/*ALsizei size,freq;
	ALenum format;
	ALvoid *data = NULL;
	ALfloat ffreq;

	//if( !strcmp( _filename + (strlen(_filename) - 4), ".wav" ) )
	{
		//alutLoadWAVMemory((ALbyte*)_buffer, &format, &data, &size, &freq, &loop);
		alutLoadMemoryFromFileImage( _buffer, _size, &format, &size, &ffreq );
		freq = ffreq;
	}

	if(data) 
	{
		buffer->setLenghtMs(size * 1000 / (freq * GetSampleSize(format)));
		alBufferData( buffer->getBufferName(), format, data, size, freq );
	} */
	return buffer;
}
//////////////////////////////////////////////////////////////////////////
Menge::SoundBufferInterface* ALSoundSystem::createSoundBuffer( Menge::DataStreamInterface* _stream, bool _isStream )
{
	if( m_initialized == false ) return 0;

	ALSoundBuffer * buffer = NULL;

	if( _isStream )
	{
		buffer = new ALSoundBufferStream();
	}
	else
	{
		buffer = new ALSoundBuffer();
	}

	if( !buffer->loadOgg( _stream ) )
	{
		delete buffer;
		buffer = 0;
	}

	return buffer;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::releaseSoundBuffer( Menge::SoundBufferInterface * _soundBuffer )
{
	//_soundBuffer->unload();
	//delete static_cast<ALSoundBuffer*>(_soundBuffer);
	delete _soundBuffer;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::releaseSoundNode( Menge::SoundSourceInterface * _sn )
{
//	delete _sn;
	if(_sn)
	{
		/*static_cast<ALSoundSource*>(_sn)->setUsed(false);
		static_cast<ALSoundSource*>(_sn)->stop();*/
		ALSoundSource* source = static_cast<ALSoundSource*>( _sn );
		source->stop();
		m_sources[ source ] = false;
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::update( float _timing )
{
	if( m_initialized == false ) return;

	//MENGE_LOG << "SoundSystem timing: " << _timing;
	//execReg_();
	for( TSourceVector::iterator it = m_stoppingSources.begin(), it_end = m_stoppingSources.end();
		it != it_end;
		it++ )
	{
		(*it)->stop();
	}
	m_stoppingSources.clear();

	m_sulk->update();

	for( TSourceVector::iterator it = m_playingSources.begin(),
			it_end = m_playingSources.end(); it != it_end; it++ )
	{
		if( (*it)->update( _timing ) == false )
		{
			m_stoppingSources.push_back( (*it ) );
		}
	}

	m_playingSources.insert( m_playingSources.end(), m_addingSources.begin(), m_addingSources.end() );
	m_addingSources.clear();

}
//////////////////////////////////////////////////////////////////////////
bool ALSoundSystem::setBlow( bool _active )
{
	if( m_sulk )
	{
		return m_sulk->activate( _active );
	}

	return false;
};
//////////////////////////////////////////////////////////////////////////
float ALSoundSystem::getBlow()
{
	if( m_sulk )
	{
		return m_sulk->getBlow();
	}

	return 0.f;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::setEnoughBlow( float _enoughBlow )
{
	if( m_sulk )
	{
		m_sulk->setEnoughBlow( _enoughBlow );
	}
};
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::setBlowCallback( Menge::SoundSulkCallbackInterface * _callback )
{
	if( m_sulk )
	{
		m_sulk->setCallback( _callback );
	}
};
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::setSoundVelocity(float _velocity)
{
	if( m_initialized == false ) return;

	m_soundVelocity = _velocity;
	alDopplerVelocity(m_soundVelocity);
}
//////////////////////////////////////////////////////////////////////////
float ALSoundSystem::getSoundVelocity()
{ 
	return m_soundVelocity; 
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::setDopplerFactor( float _factor )
{
	if( m_initialized == false ) return;

	m_dopplerFactor = _factor;
	alDopplerFactor(m_dopplerFactor);
}
//////////////////////////////////////////////////////////////////////////
float ALSoundSystem::getDopplerFactor()
{ 
	return m_dopplerFactor; 
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::setDistanceModel(EDistanceModel _model)
{
	switch(_model) 
	{
	case(None):
	  alDistanceModel(AL_NONE);
	  break;
	case(InverseDistance):
	  alDistanceModel(AL_INVERSE_DISTANCE);
	  break;
	case(InverseDistanceClamped):
	  alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	  break;
	default:
		// TODO: Error handling
		return;
	}
	if(alGetError()!=AL_FALSE)
	{
	  // TODO: Error handling
		return;
	}
	m_distanceModel = _model;
}
//////////////////////////////////////////////////////////////////////////
EDistanceModel ALSoundSystem::getDistanceModel()
{ 
	return m_distanceModel; 
}
//////////////////////////////////////////////////////////////////////////
ALuint ALSoundSystem::registerPlaying( ALSoundSource* _source, bool stereo )
{
	ALuint alSource = getFreeSourceName_( stereo );
	//m_playingSources.push_back( _source );
	m_addingSources.push_back( _source );
	return alSource;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::unregisterPlaying( ALSoundSource* _source, ALuint _alSource )
{
	TSourceVector::iterator it_find = std::find( m_playingSources.begin(), m_playingSources.end(), _source );
	if( it_find != m_playingSources.end() )
	{
		m_playingSources.erase( it_find );
		freeSource_( _alSource );
		return;
	}
	it_find = std::find( m_addingSources.begin(), m_addingSources.end(), _source );
	if( it_find != m_addingSources.end() )
	{
		m_addingSources.erase( it_find );
		freeSource_( _alSource );
	}
}
//////////////////////////////////////////////////////////////////////////
ALuint ALSoundSystem::getFreeSourceName_( bool stereo )
{
	TALSourceMap* sources;
	if( stereo )
	{
		sources = &m_alSourcesStereo;
	}
	else
	{
		sources = &m_alSourcesMono;
	}

	for( TALSourceMap::iterator it = sources->begin(), it_end = sources->end();
		it != it_end; it++ )
	{
		if( it->second == false )
		{
			it->second = true;
			return it->first;
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::freeSource_( ALuint _source )
{
	if( _source == 0 ) return;

	TALSourceMap::iterator it_find = m_alSourcesStereo.find( _source );
	if( it_find != m_alSourcesStereo.end() )
	{
		it_find->second = false;
	}
	else
	{
		m_alSourcesMono[_source] = false;
	}
}
//////////////////////////////////////////////////////////////////////////
