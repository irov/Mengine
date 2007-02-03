#	include "SoundSourceStream.h"
#	include "OpenAL/al.h"
#	include <assert.h>
#	include <windows.h>

#	include <string>

//OAL проверка ошибок
ALboolean check_error_oal1()
{
	ALenum ErrCode;
	std::string Err = "OpenAL error: ";
	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
		Err += (char *)alGetString(ErrCode);
		MessageBox(NULL, Err.data(), "Error", MB_OK | MB_ICONEXCLAMATION);
		return AL_FALSE;
	}
	return AL_TRUE;
}

OpenALSoundSourceStream::OpenALSoundSourceStream(SoundNodeListenerInterface* _plr,
												 SoundBufferInterface* _data)
	 : m_looping(false)
	 , m_headMode(false)
	 , m_vol(1.0f)
	 , m_maxDistance(1.0f)
	 , m_startTime(clock_t(0))
	 , m_listener(_plr)
	 , m_dataBuffer(_data)
{
	if (m_dataBuffer == 0)
	{
		assert(!"OpenALSoundSource error");
		return;
	}
	
	if (m_dataBuffer->getNumChannels() == 2)		
	{
		m_format = (m_dataBuffer->getBPS() == 16) ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
	}
	else
	{
		m_format = (m_dataBuffer->getBPS() == 16) ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
	}

	//Регистрируем звуковой источник, и назначаем id источнику.
	alGenSources(1,&m_source);
	check_error_oal1();

	m_pos[0] = 0;
	m_pos[1] = 0;
	m_pos[2] = 0;

	alSourcef(m_source, AL_GAIN, 1.0f);
	alSourcef(m_source, AL_PITCH, 1.0f);
	alSourcei(m_source, AL_LOOPING, m_looping);
	alSourcefv(m_source, AL_POSITION, m_pos);
	alSourcefv(m_source, AL_VELOCITY, m_pos);

	alSourcei(m_source, AL_LOOPING, AL_FALSE);

	//Генерируем STREAMFRAGMENTS буферов.
	alGenBuffers(STREAMFRAGMENTS, buffers);
	check_error_oal1();

	try
	{
		for(size_t i = 0; i < STREAMFRAGMENTS; ++i) 
		{
			_fillBufferAndQueue(buffers[i]);
		}
	} 
	catch(...)
	{
		alDeleteBuffers(STREAMFRAGMENTS, buffers);
	}
}

OpenALSoundSourceStream::~OpenALSoundSourceStream()
{
	stop();
}

bool OpenALSoundSourceStream::process()
{
	ALint state;

	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	//printf("%d \n", state);

	if (state == AL_STOPPED)
	{
		m_startTime = clock_t(0);

		if(m_listener)
		{
			m_listener->listenStoped( this );
		}
	
		return false;
	}


	int				Processed = 0;
	ALuint			BufID;

	alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &Processed);

	// We still have processed buffers
	while (Processed--)
	{
		alSourceUnqueueBuffers(m_source, 1, &BufID);
		//if (!CheckALError()) return;
	//	if (_fillBufferAndQueue(DYNBUF_SIZE) != 0)
		{
			alSourceQueueBuffers(m_source, 1, &BufID);
			//if (!CheckALError()) return;
		}
//		else
		{
			//ov_pcm_seek(mVF, 0);
			alSourceQueueBuffers(m_source, 1, &BufID);
			//if (!CheckALError()) return;

		//	if (!mLooped)
		//{
		////		Stop();
		///	}
		}
	}

	/*ALint processed = 0;

	alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

	while(processed > 0)
	{
		processed--;

		ALuint buffer;

		alSourceUnqueueBuffers(m_source, 1, &buffer);
	
		if(_fillBufferAndQueue(buffer) == false)
		{
			if(m_listener)
			{
				m_listener->listenEnded( this );
			}
			return false;
		}
	}
*/
	return true;
}

bool OpenALSoundSourceStream::_fillBufferAndQueue(ALuint buffer)
{
	unsigned char* bufferdata = new unsigned char[STREAMFRAGMENTSIZE];

	long		size = 0;
	long		ret = -1;

	//read()

	if (size > 0)
	{
		alBufferData(buffer, m_format, bufferdata, size, m_dataBuffer->getFreq());
		alSourceQueueBuffers(m_source, 1, &buffer);
	}

	return ret > 0;
}

void	OpenALSoundSourceStream::play()
{
	ALint 	state;

	alGetSourcei(m_source, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
	{
		return;
	}

	m_startTime = clock();

	alSourcePlay(m_source);
	check_error_oal1();
}

bool	OpenALSoundSourceStream::isPlaying() const
{
	ALint 	state;

	alGetSourcei (m_source, AL_SOURCE_STATE, &state);

	return	(state == AL_PLAYING);
}

void	OpenALSoundSourceStream::pause()
{
	alSourcePause(m_source);
}

void	OpenALSoundSourceStream::stop()
{
	if(m_listener)
	{
		m_listener->listenStoped( this );
	}

	alSourceStop(m_source);
	alSourcei(m_source, AL_BUFFER, AL_NONE);
	m_startTime = clock_t(0);

	delete m_dataBuffer;

	m_dataBuffer = NULL;
	alDeleteSources(1, &m_source);
	alDeleteBuffers(STREAMFRAGMENTS, buffers);
}
//////////////////////////////////////////////////////////////////////////
double	OpenALSoundSourceStream::getTotalSize()	const
{
	return	m_dataBuffer->ms_size();
}
//////////////////////////////////////////////////////////////////////////
double	OpenALSoundSourceStream::getPos()	const
{
	clock_t m_endTime = clock();
	return (m_endTime - m_startTime) / CLOCKS_PER_SEC;
}

void	OpenALSoundSourceStream::setLooped(bool _flag)	
{
	assert("!Stream sound can't be looped!");
	//m_looping = _flag; 
	//alSourcei(m_source, AL_LOOPING, m_looping);
}

void	OpenALSoundSourceStream::setVolume(float _value)
{
	m_vol = _value;
	alSourcef(m_source, AL_GAIN, m_vol);
}

void	OpenALSoundSourceStream::setPosition(const float* _position)
{
	m_pos[0] = _position[0];
	m_pos[1] = _position[1];
	m_pos[2] = _position[2];
	alSourcefv(m_source, AL_POSITION, m_pos);
}

void	OpenALSoundSourceStream::setMaxDistance(float _dist)	
{
	m_maxDistance = _dist;
	alSourcef(m_source, AL_MAX_DISTANCE, m_maxDistance);
}

void	OpenALSoundSourceStream::setHeadMode(bool _flag) 
{
	m_headMode = _flag;
	alSourcei(m_source, AL_SOURCE_RELATIVE, m_headMode);
}

const float*	OpenALSoundSourceStream::getPosition() const
{
	return	m_pos;
}

float	OpenALSoundSourceStream::getMaxDistance() const
{
	return	m_maxDistance;
}

float	OpenALSoundSourceStream::getVolume() const
{
	return	m_vol;
}

bool	OpenALSoundSourceStream::getHeadMode() const
{
	return	m_headMode;
}

bool	OpenALSoundSourceStream::getLooped() const
{
	return	m_looping;
}