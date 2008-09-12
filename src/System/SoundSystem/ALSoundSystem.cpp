

#	include "ALSoundBuffer.h"
#	include "ALSoundBufferStream.h"
#	include "ALSoundSource.h"
#	include "ALSoundSystem.h"

#	include "SulkSystem.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
ALSoundSystem::ALSoundSystem()
: m_device(0) 
, m_soundVelocity(343.0f)
, m_dopplerFactor(1.0f)
, m_distanceModel(None)
, m_sourceNamesNum(0)
, m_initialized( false )
{
}
//////////////////////////////////////////////////////////////////////////
ALSoundSystem::~ALSoundSystem()
{
	if( m_initialized )
	{
		for( TSourceVector::size_type i = 0; i < m_sources.size(); i++ )
		{
			delete m_sources[i];
		}
		m_sources.clear();

		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_context);
		alcCloseDevice(m_device);

		delete m_sulk;
	}
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundSystem::initialize()
{
	if( m_initialized )
	{
		return false;
	}

	//char *initString = 0L;//"DirectSound", "DirectSound3D", ;
	m_device = alcOpenDevice( NULL );

	if( !m_device )
	{
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
		return false;
	} 

	alcMakeContextCurrent( m_context );

	//
	m_soundVelocity = 343.0f;
	m_dopplerFactor = 1.0f;
	m_distanceModel = None;
	m_sourceNamesNum = 0;

	for(;m_sourceNamesNum < MAX_SOURCENAMES_NUM; m_sourceNamesNum++)
	{
		ALuint sourceName;
		alGenSources(1, &sourceName);	
		if(alGetError())
			break;
		m_sourceNames[m_sourceNamesNum].busy = false;
		m_sourceNames[m_sourceNamesNum].name = sourceName;
	}
	m_sources.reserve(MAX_SOUND_SOURCES);
	m_deletingSources.reserve(MAX_SOUND_SOURCES);

	// Environment settings 
	alSpeedOfSound(m_soundVelocity);
	alDopplerFactor(m_dopplerFactor);
	setDistanceModel(m_distanceModel);

	m_sulk = new SulkSystem();

	m_initialized = true;
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
	//ALSoundSource* source = new ALSoundSource(this);
	ALSoundSource* source = NULL;

	for(unsigned int i = 0; i < m_sources.size(); i++)
	{
		if(!m_sources[i]->isBusy())
		{
			source = m_sources[i];
			break;
		}
	}

	if(!source)
	{
		source = new ALSoundSource(this);
		m_sources.push_back(source);
	}

	source->setSoundBuffer( static_cast<ALSoundBuffer*>(_sample) );
	source->setAmbient( _isHeadMode );
	source->setSoundNodeListener( _listener );
	source->setUsed( true );

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
		static_cast<ALSoundSource*>(_sn)->setUsed(false);
		static_cast<ALSoundSource*>(_sn)->stop();
	}
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::update( float _timing )
{

	if( m_initialized == false ) return;

	for( TSourceVector::iterator it = m_deletingSources.begin(), 
		it_end = m_deletingSources.end(); it != it_end; it++ )
	{
		m_playingSources.erase( (*it) );
	}
	m_deletingSources.clear();

	for( std::size_t i = 0; i < m_addingSources.size(); i++ )
	{
		m_playingSources[ m_addingSources[i].first ] = m_addingSources[i].second;
	}
	m_addingSources.clear();

	for(unsigned int i = 0; i < m_streams.size(); i++)
	{
		m_streams[i]->update();
	}

	m_sulk->update();


	for( TSourcesMap::iterator it = m_playingSources.begin(),
			it_end = m_playingSources.end(); it != it_end; it++ )
	{
		it->second -= _timing;
		if( it->second <= 0 )
		{
			it->first->unbind();
		}
	}

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
void ALSoundSystem::addStream( ALSoundBufferStream *_stream )
{
	m_streams.push_back( _stream );
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::removeStream( ALSoundBufferStream* _stream )
{
	TVectorALSoundBufferStream::iterator it_find = std::find( m_streams.begin(), m_streams.end(), _stream );

	if( it_find != m_streams.end() )
	{
		m_streams.erase( it_find );
	}
}
//////////////////////////////////////////////////////////////////////////
TALSourceName* ALSoundSystem::getFreeSourceName( bool stereo )
{
	int i = stereo ? 0 : (m_sourceNamesNum / 2);
	int count = stereo ? (m_sourceNamesNum / 2) : m_sourceNamesNum;
	for( ; i < count; i++)
	{
		if(!m_sourceNames[i].busy)
		{
			return &m_sourceNames[i];
		}
	}
	i = stereo ? 0 : (m_sourceNamesNum / 2);

	for( ; i < count; i++)
	{
		int state;
		alGetSourcei(m_sourceNames[i].name, AL_SOURCE_STATE, &state);
		if(state != AL_PLAYING)
		{
			return &m_sourceNames[i];
		}
	}
	
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::registerPlaying( ALSoundSource* _source, float _timeMs )
{
	//m_playingSources[_source] = _timeMs;
	m_addingSources.push_back( std::make_pair( _source, _timeMs ) );
}
//////////////////////////////////////////////////////////////////////////
void ALSoundSystem::unregisterPlaying( ALSoundSource* _source )
{
	m_deletingSources.push_back( _source );
}
//////////////////////////////////////////////////////////////////////////
