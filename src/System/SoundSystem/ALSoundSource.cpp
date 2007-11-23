
#include "stdafx.h"

#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

void CALLBACK ListenStoppedCallback(void* _listener, BOOL)
{
	if(_listener)
	{
		static_cast<SoundNodeListenerInterface*>(_listener)->listenStopped();
	}
}

ALSoundSource::ALSoundSource(ALSoundSystem* _soundSystem) :
m_soundBuffer(NULL),
m_listener(NULL),
m_sourceName(AL_INVALID),
m_soundSystem(_soundSystem),
m_busy(false)
{
	ZeroMemory(m_position, sizeof(float)*3);
}

ALSoundSource::~ALSoundSource()
{
	if(isPlaying())
	{
		stop();
	}
}

void ALSoundSource::play()
{
	m_sourceName = m_soundSystem->getFreeSourceName();

	if(m_soundBuffer && m_soundBuffer->isStreamed())
	{
		alSourcei(m_sourceName, AL_BUFFER, NULL);
	    alSourcei(m_sourceName, AL_LOOPING, AL_FALSE); //Streaming sources can't loop
		static_cast<ALSoundBufferStream*>(m_soundBuffer)->record(m_sourceName);
	}
	else
	{
		alSourcei(m_sourceName, AL_BUFFER, m_soundBuffer->getBufferName());
		alSourcePlay(m_sourceName);
	}


	SetTimerQueueTimer(NULL, (WAITORTIMERCALLBACK)ListenStoppedCallback, m_listener, getLengthMs(), 0, 0);
}

void ALSoundSource::pause()
{
	alSourcePause(m_sourceName);
	if(m_listener)
	{
		m_listener->listenPaused(true);
	}
}

void ALSoundSource::stop()
{
	if(m_soundBuffer && m_soundBuffer->isStreamed())
	{
		static_cast<ALSoundBufferStream*>(m_soundBuffer)->stop();
	}
	else
	{
		alSourceStop(m_sourceName);
	}

	/*if(m_listener)
	{
		m_listener->listenStopped();
	}*/
}

bool ALSoundSource::isPlaying() const
{
  ALint state;
  alGetSourceiv(m_sourceName,AL_SOURCE_STATE,&state);
  if(state == AL_PLAYING)
  {
	  return true;
  }
  return false;
}

void ALSoundSource::setVolume( float _volume )
{
	alSourcef(m_sourceName, AL_GAIN, _volume);
}

float ALSoundSource::getVolume() const
{
	ALfloat gain;
	alGetSourcefv(m_sourceName, AL_GAIN, &gain);
	return gain;
}

void ALSoundSource::setPosition( float _x, float _y, float _z )
{
	alSource3f(m_sourceName, AL_POSITION, _x, _y, _z);
	m_position[0] = _x;
	m_position[1] = _y;
	m_position[2] = _z;
}

const float * ALSoundSource::getPosition() const
{
	return m_position;
}

void ALSoundSource::setLooped( bool _loop )
{
	if(m_soundBuffer && m_soundBuffer->isStreamed())
	{
		static_cast<ALSoundBufferStream*>(m_soundBuffer)->getUpdater()->setLooping(_loop);
	}
	else
	{
		alSourcei(m_sourceName, AL_LOOPING, _loop ? AL_TRUE : AL_FALSE);
	}
}

bool ALSoundSource::isLooping() const
{
	if(m_soundBuffer && m_soundBuffer->isStreamed())
	{
		return static_cast<ALSoundBufferStream*>(m_soundBuffer)->getUpdater()->isLooping();
	}
	// else
	ALint looping;
	alGetSourceiv(m_sourceName, AL_LOOPING, &looping);
	return (looping == AL_TRUE);
}

int ALSoundSource::getLengthMs()
{
	if(m_soundBuffer)
	{
		return m_soundBuffer->getLenghtMs();
	}
	return 0;
}

int ALSoundSource::getPosMs()
{
	ALint pos;
	alGetSourcei(m_sourceName, AL_SEC_OFFSET, &pos);
	return pos;
}

void ALSoundSource::setSoundBuffer( ALSoundBuffer* _soundBuffer )
{
	m_soundBuffer = _soundBuffer;
}

void ALSoundSource::setAmbient(bool _ambient )
{
	if(_ambient)
	{
		alSourcei(m_sourceName, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(m_sourceName, AL_ROLLOFF_FACTOR, 0.0f);
		alSource3f(m_sourceName, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
	} 
	else 
	{
		alSourcei(m_sourceName, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(m_sourceName, AL_ROLLOFF_FACTOR, 1.0f);
	}

}
