#	include "ALOggStreamSound.h"
#	include "assert.h"

#ifndef OGG_BUFFER_SIZE
	#define OGG_BUFFER_SIZE 32768	//32 kb
#endif

size_t OggStreamSound::oggread(void * _ptr, size_t _size, size_t _nmemb, void * _datasource)
{
	OggStreamSound* oggf = reinterpret_cast<OggStreamSound*>(_datasource);

	size_t count_to_read = (_nmemb * _size + oggf->m_streampos < oggf->m_info._size) 
		? _nmemb : (oggf->m_info._size - oggf->m_streampos) / _size;

	memcpy(_ptr, (void*)((char*)oggf->m_info._data + oggf->m_streampos), count_to_read * _size);
	oggf->m_streampos += count_to_read * _size;
	return count_to_read;
}

int OggStreamSound::oggseek(void *_datasource, ogg_int64_t _offset, int _whence)
{
	OggStreamSound* oggf = reinterpret_cast<OggStreamSound*>(_datasource);

	ALsizei curpos = 0;

	switch(_whence)
	{
	case SEEK_SET: 
		curpos = (ALsizei)_offset; 
		break;
	case SEEK_CUR: 
		curpos = oggf->m_streampos + (ALsizei)_offset; 
		break;
	case SEEK_END: 
		curpos = oggf->m_info._size; 
		break;
	default: 
		return -1;
	}

	if(curpos < 0)
	{
		oggf->m_streampos = 0;
	}
	else 
	{
		oggf->m_streampos = (curpos > oggf->m_info._size) ? oggf->m_info._size : curpos;
	}

	return 0;
}

long OggStreamSound::oggtell(void *_datasource)
{
	OggStreamSound* oggf = reinterpret_cast<OggStreamSound*>(_datasource);
	return (long)oggf->m_streampos;
}

int OggStreamSound::oggclose(void *_datasource)
{
	return 0;
}

OggStreamSound::OggStreamSound(void* _data, int _size, SoundNodeListenerInterface* _listener)
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
, m_streampos(0)
{
	printf("Ogg SoundSource Created /n");
	m_info._data = _data;
	m_info._size = _size;

	ov_callbacks vorbisCallbacks;

	vorbisCallbacks.read_func  = oggread;
	vorbisCallbacks.seek_func  = oggseek;
	vorbisCallbacks.tell_func  = oggtell;
	vorbisCallbacks.close_func = oggclose;

	if(ov_open_callbacks(this, &m_oggStream, NULL, 0, vorbisCallbacks) < 0)
	{
		assert(0);
	}

	m_vorbisInfo = ov_info(&m_oggStream, -1);
	m_vorbisComment = ov_comment(&m_oggStream, -1);

	m_info._format = (m_vorbisInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	m_info._frequency = m_vorbisInfo->rate;

	alGenBuffers(2, m_buffers);
	ALSoundSystem::checkError();

	alGenSources(1, &m_source);
	ALSoundSystem::checkError();

	for(int i = 0; i < 2; i++)
	{
		if(!oggstream(m_buffers[i]))
		{
			assert(!"Could not open Ogg file.");
		}
	}

	alSourceQueueBuffers(m_source, 2, m_buffers);
	ALSoundSystem::checkError();

	alSourcef (m_source, AL_PITCH,				m_pitch);
	alSourcef (m_source, AL_GAIN,				m_gain);
	alSourcef (m_source, AL_MAX_GAIN,			m_maxGain);
	alSourcef (m_source, AL_MIN_GAIN,			m_minGain);
	alSourcef (m_source, AL_MAX_DISTANCE,		m_maxDistance);
	alSourcef (m_source, AL_ROLLOFF_FACTOR,		m_rolloffFactor);
	alSourcef (m_source, AL_REFERENCE_DISTANCE,	m_refDistance);
	alSourcef (m_source, AL_CONE_OUTER_GAIN,	m_outerConeGain);
	alSourcef (m_source, AL_CONE_INNER_ANGLE,	m_innerConeAngle);
	alSourcef (m_source, AL_CONE_OUTER_ANGLE,	m_outerConeAngle);
	alSource3f(m_source, AL_POSITION,			0,0,0);
	alSource3f(m_source, AL_VELOCITY,			0,0,0);
	alSource3f(m_source, AL_DIRECTION,			0,0,0);
	alSourcei (m_source, AL_SOURCE_RELATIVE,	m_sourceRelative);
	alSourcei (m_source, AL_LOOPING,			AL_FALSE);
	ALSoundSystem::checkError();

	m_pos[0] = m_pos[1] = m_pos[2] = 0;
	m_vel[0] = m_vel[1] = m_vel[2] = 0;
	m_dir[0] = m_dir[1] = m_dir[2] = 0;
}

double	OggStreamSound::getSizeSec()
{
	return ov_time_total(&m_oggStream,-1);
}

bool OggStreamSound::oggstream(ALuint buffer)
{
	char data[OGG_BUFFER_SIZE];
	ALint size = 0;
	ALint section;
	ALint result;

	while(size < OGG_BUFFER_SIZE)
	{
		result = ov_read(&m_oggStream, data+size, OGG_BUFFER_SIZE - size, 0, 2, 1, &section);

		if(result > 0)
		{
			size += result;
		}
		else
			if(result < 0)
			{
				assert(0);
			}
			else break;
	}

	if(size == 0)
	{
		return false;
	}

	alBufferData(buffer, m_info._format, data, size, m_info._frequency);
	ALSoundSystem::checkError();

	return true;
}

bool OggStreamSound::process()
{
	ALint processed;
	bool active = true;

	alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
	ALSoundSystem::checkError();
//падает после второго вызова фейд дауна
printf("%d \n", processed);
	while(processed--)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(m_source, 1, &buffer);
		ALSoundSystem::checkError();

		active = oggstream(buffer);

		alSourceQueueBuffers(m_source, 1, &buffer);
		ALSoundSystem::checkError();

		if(!active)
		{
			if(m_loop)
			{
				active = true;
				ov_time_seek(&m_oggStream, 0);
			}
			else
			{
				stop();
			}
		}
	}

	return active;
}

OggStreamSound::~OggStreamSound()
{
	printf("Ogg SoundSource Killed /n");
	// If the sound doens't have a state yet it causes an
	// error when you try to unqueue the buffers! :S
	if(isInitial()) 
	{
		play();
	}

	stop();

	ALint queued;

	alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
	alSourceUnqueueBuffers(m_source, queued, m_buffers);
	ALSoundSystem::checkError();

	alDeleteBuffers(2, m_buffers);
	ov_clear(&m_oggStream);

	alSourceStop(m_source);
	alSourcei(m_source, AL_BUFFER, 0);

	ALSoundSystem::checkError();

	m_vorbisInfo = 0;
	m_vorbisComment = 0;
}

bool OggStreamSound::play()
{
	m_startTime = clock();
	printf("Begin To Play /n");
	if(isPlaying())
	{
		return true;
	}
	else if(isStopped())
	{
		for(int i = 0; i < 2; i++)
		{
			if(!oggstream(m_buffers[i]))
			{
				return false;
			}
		}

		alSourceQueueBuffers(m_source, 2, m_buffers);
		alSourcePlay(m_source);
		ALSoundSystem::checkError();
		return true;
	}
	else
	{
		alSourcePlay(m_source);
		ALSoundSystem::checkError();
		return true;
	}
}

bool OggStreamSound::pause()
{
	if(!isPlaying())
	{
		return true;
	}

	alSourcePause(m_source);

	if (m_listener)
	{
		m_listener->listenPaused();
	}
	return true;
}

bool OggStreamSound::stop()
{
	m_startTime = clock_t(0);

	if(isStopped() || isInitial())
	{
		return true;
	}

	int queued;
	alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
	ALSoundSystem::checkError();

	alSourceStop(m_source);
	ALSoundSystem::checkError();

	alSourceUnqueueBuffers(m_source, queued, m_buffers);
	ALSoundSystem::checkError();

	ov_time_seek(&m_oggStream, 0);

	if (m_listener)
	{
		m_listener->listenStoped();
	}
	return true;
}

bool OggStreamSound::isPlaying() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_PLAYING);
}

bool OggStreamSound::isPaused() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_PAUSED);
}

bool OggStreamSound::isStopped() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_STOPPED);
}

bool OggStreamSound::isInitial() const
{
	ALenum state;    
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);    
	return (state == AL_INITIAL);
}

void OggStreamSound::setPitch(float pitch)
{
	m_pitch = pitch;
	alSourcef(m_source, AL_PITCH, m_pitch);
}

void OggStreamSound::setGain(float gain)
{
	m_gain = gain;
	alSourcef(m_source, AL_GAIN, m_gain);
}

void OggStreamSound::setRelativeToListener(bool relative)
{
	m_sourceRelative = relative;
	alSourcei(m_source, AL_SOURCE_RELATIVE, m_sourceRelative);
}

void OggStreamSound::setLoop(bool loop)
{
	m_loop = loop ? AL_TRUE : AL_FALSE;
}

void OggStreamSound::setPosition(float x, float y, float z)
{
	m_pos[0] = x;
	m_pos[1] = y;
	m_pos[2] = z;
}

const float* OggStreamSound::getPosition() const
{
	return m_pos;
}

void OggStreamSound::setDirection(float x, float y, float z)
{
	m_dir[0] = x;
	m_dir[1] = y;
	m_dir[2] = z;
}

const float* OggStreamSound::getDirection() const
{
	return m_dir;
}

float OggStreamSound::getPitch() const
{
	return m_pitch;
}

float OggStreamSound::getGain() const
{
	return m_gain;
}

bool OggStreamSound::isRelativeToListener() const
{
	return m_sourceRelative == AL_TRUE ? true : false;
}

bool OggStreamSound::isLooping() const
{
	return m_loop == AL_TRUE ? true : false;
}