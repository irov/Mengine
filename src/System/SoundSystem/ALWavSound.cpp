#	include "ALWavSound.h"
#	include "OpenAL/al.h"
#	include "OpenAL/alut.h"

double	WavSound::getSizeSec()
{
/*	double byte_per_sample = getBPS() * getNumChannels() / 8;

	assert(byte_per_sample>=0);

	double freq = getFreq();

	assert(freq>=0);

	return m_dataLength / byte_per_sample / freq;
*/
	return 0;
}

WavSound::WavSound(void* _data, SoundNodeListenerInterface* _listener)
: m_listener(_listener)
, m_pitch(1.0)
, m_gain(1.0)
, m_maxGain(1.0)
, m_minGain(0.0)
, m_maxDistance(3400.0)
, m_rolloffFactor(1.0)
, m_refDistance(150.0)
, m_outerConeGain(0.0)
, m_innerConeAngle(360.0)
, m_outerConeAngle(360.0)
, m_sourceRelative(AL_FALSE)
, m_loop(AL_FALSE)
{
	alGenBuffers(1, &m_buffer);
	ALSoundSystem::checkError();

	alutLoadWAVMemory(
		reinterpret_cast<ALbyte*>(_data),
		&m_info._format,
		&m_info._data, 
		&m_info._size,
		&m_info._frequency,
		&m_loop);

	alBufferData(m_buffer, m_info._format, m_info._data, m_info._size, m_info._frequency);

	alutUnloadWAV(m_info._format, m_info._data, m_info._size, m_info._frequency);

	alGenSources(1, &m_source);
	ALSoundSystem::checkError();

	alSourcei(m_source, AL_BUFFER, m_buffer);
	ALSoundSystem::checkError();

	alSourcef (m_source, AL_PITCH,				m_pitch);
	alSourcef (m_source, AL_GAIN,				m_gain);
	alSourcef (m_source, AL_MAX_GAIN,			m_maxGain);
	alSourcef (m_source, AL_MIN_GAIN,			m_minGain);
	alSourcef (m_source, AL_MAX_DISTANCE,		m_maxDistance);
	alSourcef (m_source, AL_ROLLOFF_FACTOR,		m_rolloffFactor);
	alSourcef (m_source, AL_REFERENCE_DISTANCE,	m_refDistance);
	alSourcef (m_source, AL_CONE_OUTER_GAIN,		m_outerConeGain);
	alSourcef (m_source, AL_CONE_INNER_ANGLE,	m_innerConeAngle);
	alSourcef (m_source, AL_CONE_OUTER_ANGLE,	m_outerConeAngle);
	alSource3f(m_source, AL_POSITION,			0,0,0);
	alSource3f(m_source, AL_VELOCITY,			0,0,0);
	alSource3f(m_source, AL_DIRECTION,			0,0,0);
	alSourcei (m_source, AL_SOURCE_RELATIVE,		m_sourceRelative);
	alSourcei (m_source, AL_LOOPING,				m_loop);
	ALSoundSystem::checkError();

	m_pos[0] = m_pos[1] = m_pos[2] = 0;
	m_vel[0] = m_vel[1] = m_vel[2] = 0;
	m_dir[0] = m_dir[1] = m_dir[2] = 0;
}

WavSound::~WavSound()
{
	alSourceStop(m_source);
	alSourcei(m_source, AL_BUFFER, 0);
	alDeleteBuffers(1, &m_buffer);
	ALSoundSystem::checkError();
}

bool WavSound::process()
{
	return true;
}

bool WavSound::play()
{
	if(isPlaying()) return true;

	alSourcePlay(m_source);	    
	return true;
}

bool WavSound::isPlaying() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_PLAYING);
}

bool WavSound::pause()
{
	if(!isPlaying()) return true;

	alSourcePause(m_source);

	if (m_listener)
	{
		m_listener->listenPaused();
	}
	return true;
}

bool WavSound::isPaused() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_PAUSED);
}

bool WavSound::stop()
{
	if(isStopped()) return true;

	alSourceStop(m_source);

	if (m_listener)
	{
		m_listener->listenStoped();
	}

	return true;
}

bool WavSound::isStopped() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_STOPPED);
}

bool WavSound::isInitial() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_INITIAL);
}

void WavSound::setPitch(float pitch)
{
	m_pitch = pitch;
	alSourcef(m_source, AL_PITCH, m_pitch);
}

void WavSound::setGain(float gain)
{
	m_gain = gain;
	alSourcef(m_source, AL_GAIN, m_gain);
}

void WavSound::setRelativeToListener(bool relative)
{
	m_sourceRelative = relative;
	alSourcei(m_source, AL_SOURCE_RELATIVE, m_sourceRelative);
}

void WavSound::setLoop(bool loop)
{
	m_loop = loop?AL_TRUE:AL_FALSE;
	alSourcei(m_source, AL_LOOPING, m_loop);
}

void WavSound::setPosition(float x, float y, float z)
{
	m_pos[0] = x;
	m_pos[1] = y;
	m_pos[2] = z;
}

const float* WavSound::getPosition() const
{
	return m_pos;
}

void WavSound::setDirection(float x, float y, float z)
{
	m_dir[0]=x;
	m_dir[1]=y;
	m_dir[2]=z;
}
const float* WavSound::getDirection() const
{
	return m_dir;
}
float WavSound::getPitch() const
{
	return m_pitch;
}

float WavSound::getGain() const 
{
	return m_gain;
}

bool WavSound::isRelativeToListener() const
{
	return m_sourceRelative == AL_TRUE ? true : false;
}

bool WavSound::isLooping() const
{
	return m_loop == AL_TRUE ? true : false;
}