#	include "SoundSource.h"

#	include "SoundBuffer.h"

#	include <assert.h>

#	include "includes/OpenAL/al.h"

#	include <stdio.h>

OpenALSoundSource::OpenALSoundSource( 
									 SoundNodeListenerInterface* _plr,
									 SoundBufferInterface* _data,
									 bool _streaming)
:	SoundSourceInterface(),
    m_looping(false), 
	m_headMode(false), 
	m_vol(1.0f), 
	m_maxDistance(1),
	m_startTime(0)
{
	m_listener = _plr;
	m_dataBuffer = _data;

	if (m_dataBuffer == 0)
	{
		assert(!"OpenALSoundSource error");
		return;
	}

	m_isStream  = _streaming;

	if (m_dataBuffer->getNumChannels() == 2)		
	{
		if ( m_dataBuffer->getBitsPerSample() == 16)
		{
			m_format = AL_FORMAT_STEREO16;
		}
		else
		{
			m_format = AL_FORMAT_STEREO8;
		}
	}
	else
	{
		if (m_dataBuffer->getBitsPerSample() == 16)
		{
			m_format = AL_FORMAT_MONO16;
		}
		else
		{
			m_format = AL_FORMAT_MONO8;
		}
	}
	
	if (m_isStream)
	{
		m_dataSoundBuffer = new unsigned char [BUFFER_SIZE * 2];
		m_currentBuffer = 0;
		alGenBuffers(2, m_buffer);
		alGenSources(1, &m_source);
	}
	else
	{
		m_dataSoundBuffer = new unsigned char[m_dataBuffer->getDataSoundSize()];
		int size = m_dataBuffer->read(m_dataSoundBuffer);
		alGenBuffers(1, m_buffer);
		alBufferData(m_buffer[0], m_format, m_dataSoundBuffer, size, m_dataBuffer->getFrequency());
		alGenSources(1,&m_source);
		alSourcei(m_source, AL_BUFFER, m_buffer[0]);
		delete m_dataSoundBuffer;
	}

	m_pos[0] = 0;
	m_pos[1] = 0;
	m_pos[2] = 0;
	float m_ov[3] = {};
	alSourcef(m_source, AL_GAIN, 1.0f);
	alSourcef(m_source, AL_PITCH, 1.0f);
	alSourcei(m_source, AL_LOOPING, m_looping);
	alSourcefv(m_source, AL_POSITION,  m_pos);
	alSourcefv(m_source, AL_VELOCITY,  m_ov);
	alSourcei(m_source, AL_SOURCE_RELATIVE, m_headMode);
}

OpenALSoundSource::~OpenALSoundSource()
{
	printf("SoundSource Cleared \n");
	release();
}
void	OpenALSoundSource::release()
{
	stop();
	delete m_dataBuffer;

	if (m_isStream)
	{
		delete m_dataSoundBuffer;
	}

	alDeleteSources(1, &m_source);

	if (m_isStream)
	{
		alDeleteBuffers(2,m_buffer);
	}
	else
	{
		alDeleteBuffers(1,m_buffer);
	}
}

void	OpenALSoundSource::play()
{
	ALint 	state;
	int		size;
	alGetSourcei (m_source, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
	{
		return;
	}

	m_startTime = clock();

	if (state != AL_PAUSED && m_isStream)
	{
		size = m_dataBuffer->read(m_dataSoundBuffer, BUFFER_SIZE);
		alBufferData(m_buffer[0], m_format, m_dataSoundBuffer, size, m_dataBuffer->getFrequency());
		size = m_dataBuffer->read ( m_dataSoundBuffer, BUFFER_SIZE );
		alBufferData(m_buffer[1], m_format, m_dataSoundBuffer, size, m_dataBuffer->getFrequency());
		alSourceQueueBuffers(m_source, 2, m_buffer);
	}
	alSourcePlay (m_source);
}

bool	OpenALSoundSource::isPlaying() const
{
	ALint 	state;

	alGetSourcei (m_source, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
	{
		return	true;
	}

	return	false;
}

void	OpenALSoundSource::pause()
{
	alSourcePause(m_source);
}

void	OpenALSoundSource::stop()
{
	alSourceStop(m_source);
	m_dataBuffer->seek(0);

	if (m_isStream)
	{
		ALint queued;
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
		if (queued > 0)
		{
			alSourceUnqueueBuffers(m_source, 2, m_buffer);
		}
		m_currentBuffer = 0;
		if(m_listener)
		{
			m_listener->listenStoped(this);
		}
	}
	m_startTime = 0;
}
//////////////////////////////////////////////////////////////////////////
double		OpenALSoundSource::getTotalSize()	const
{
	return	m_dataBuffer->getTotalTime();
}
//////////////////////////////////////////////////////////////////////////
double		OpenALSoundSource::getPos()	const
{
	clock_t m_endTime = clock();
	return (m_endTime - m_startTime) / CLOCKS_PER_SEC;
	//return	mBufferWithData->getTotalTime();
}
//////////////////////////////////////////////////////////////////////////
bool	 OpenALSoundSource::updateSoundBuffer()
{
	ALint state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	
	if (state == AL_STOPPED && m_isStream)
	{
		m_startTime = 0;

		if(m_listener)
		{
			m_listener->listenEnded( this );
		}
	
		return false;
	}

	if (state == AL_PLAYING && m_dataBuffer->atEnd() && getLooped())
	{
		if(m_listener)
		{
			m_listener->listenRecycled(this);
		}
	}

	if (m_isStream)
	{
		if(m_looping)
		{
			assert(!"Stream Sound can't be looped!");
		}

		ALint processed;
		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

		if (processed == 0)
		{
			return true;
		}

		if (processed == 1)
		{
			alSourceUnqueueBuffers(m_source, 1, &m_buffer[m_currentBuffer]);
			int size = m_dataBuffer->read(m_dataSoundBuffer,BUFFER_SIZE);
			if (size > 0 || (size == 0 && m_looping))
			{
				alBufferData(m_buffer[m_currentBuffer], m_format, m_dataSoundBuffer, size, m_dataBuffer->getFrequency());
				alSourceQueueBuffers(m_source, 1, &m_buffer[m_currentBuffer]);
				if (size < BUFFER_SIZE && m_looping)
				{
					m_dataBuffer->seek(0);
				};
			}
			else
			{
				int queued;

				alGetSourcei(m_source,AL_BUFFERS_QUEUED,&queued);

				if (queued == 0)
				{
					m_dataBuffer -> seek(0);
				}
			}
			m_currentBuffer = 1 - m_currentBuffer;
		}
		else
		if (processed == 2)
		{
			alSourceUnqueueBuffers(m_source, 2, m_buffer);
			m_currentBuffer = 0;
			play();
		}
	}
	return true;
}

void	OpenALSoundSource::setLooped(bool _flag)	
{
	m_looping = _flag; 
	alSourcei(m_source, AL_LOOPING, m_looping);
}

void	OpenALSoundSource::setVolume(float _value)
{
	m_vol = _value;
	alSourcef(m_source, AL_GAIN, m_vol);
}

void	OpenALSoundSource::setPosition(const float* _position)
{
	m_pos[0] = _position[0];
	m_pos[1] = _position[1];
	m_pos[2] = _position[2];
	alSourcefv(m_source, AL_POSITION, m_pos);
}

void		OpenALSoundSource::setMaxDistance(float _dist)	
{
	m_maxDistance = _dist;
	alSourcef(m_source, AL_MAX_DISTANCE, m_maxDistance);
}

void		OpenALSoundSource::setHeadMode(bool _flag) 
{
	m_headMode = _flag;
	alSourcei(m_source, AL_SOURCE_RELATIVE, m_headMode);
}

const float*	OpenALSoundSource::getPosition() const
{
	return	m_pos;
}

float			OpenALSoundSource::getMaxDistance() const
{
	return	m_maxDistance;
}

float			OpenALSoundSource::getVolume() const
{
	return	m_vol;	
}

bool			OpenALSoundSource::getHeadMode() const	
{
	return	m_headMode;	
}

bool			OpenALSoundSource::getLooped() const
{
	return	m_looping;	
}