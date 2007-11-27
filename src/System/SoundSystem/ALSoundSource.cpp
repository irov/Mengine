
#include "stdafx.h"

#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

void CALLBACK ListenStoppedCallback(void* _source, BOOL)
{
	ALSoundSource* source = static_cast<ALSoundSource*>(_source);
	SoundNodeListenerInterface* listener = source->getListener();
	if(source->isPlaying())
		source->stop();
	/*if(source->isPlaying() && listener)
	{
		listener->listenStopped();
	}*/
}

ALSoundSource::ALSoundSource(ALSoundSystem* _soundSystem) :
m_soundBuffer(NULL),
m_listener(NULL),
m_sourceName(NULL),
m_soundSystem(_soundSystem),
m_busy(false),
m_playing(false),
m_looped(false),
m_volume(1.0f)
{
	ZeroMemory(m_position, sizeof(float)*3);
}

ALSoundSource::~ALSoundSource()
{
	stop();
	if(m_soundBuffer)
		m_soundBuffer->removeSource(this);
}

void ALSoundSource::play()
{
	m_sourceName = m_soundSystem->getFreeSourceName();
	if(!m_sourceName) 
	{
		return;
	}

	m_sourceName->busy = true;

	_updateParams();

	if(m_soundBuffer && m_soundBuffer->isStreamed())
	{
		alSourcei(m_sourceName->name, AL_BUFFER, NULL);
	    alSourcei(m_sourceName->name, AL_LOOPING, AL_FALSE); //Streaming sources can't loop
		static_cast<ALSoundBufferStream*>(m_soundBuffer)->record(m_sourceName->name);
	}
	else
	{
		alSourcei(m_sourceName->name, AL_BUFFER, m_soundBuffer->getBufferName());
		alSourcePlay(m_sourceName->name);
	}


	SetTimerQueueTimer(NULL, (WAITORTIMERCALLBACK)ListenStoppedCallback, this, getLengthMs(), 0, 0);
	m_playing = true;
}

void ALSoundSource::pause()
{
	alSourcePause(m_sourceName->name);

	if(m_listener)
	{
		m_listener->listenPaused(true);
	}
}

void ALSoundSource::stop()
{
	m_playing = false;

	if(!m_sourceName)
		return;

	if(m_soundBuffer && m_soundBuffer->isStreamed())
	{
		static_cast<ALSoundBufferStream*>(m_soundBuffer)->stop();
	}
	else
	{
		alSourceStop(m_sourceName->name);
		alSourcei(m_sourceName->name, AL_BUFFER, NULL);
	}

	m_sourceName->busy = false;

	if(m_busy && m_listener)
	{
		m_listener->listenStopped();
	}
}

void ALSoundSource::setPosition( float _x, float _y, float _z )
{
	m_position[0] = _x;
	m_position[1] = _y;
	m_position[2] = _z;
}

const float * ALSoundSource::getPosition() const
{
	return m_position;
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
	alGetSourcei(m_sourceName->name, AL_SEC_OFFSET, &pos);

	return pos;
}

void ALSoundSource::setSoundBuffer( ALSoundBuffer* _soundBuffer )
{
	m_soundBuffer = _soundBuffer;
	if(m_soundBuffer)
		m_soundBuffer->addSource(this);
}

void ALSoundSource::setVolume(float _volume)
{
	m_volume = _volume;

	if(m_sourceName && m_playing)
	{
		alSourcef(m_sourceName->name, AL_GAIN, m_volume);
	}
}

void ALSoundSource::_updateParams()
{
	if(m_ambient)
	{
		alSourcei(m_sourceName->name, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(m_sourceName->name, AL_ROLLOFF_FACTOR, 0.0f);
		alSource3f(m_sourceName->name, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
	} 
	else 
	{
		alSourcei(m_sourceName->name, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(m_sourceName->name, AL_ROLLOFF_FACTOR, 1.0f);
	}

	if(m_soundBuffer && m_soundBuffer->isStreamed())
	{
		static_cast<ALSoundBufferStream*>(m_soundBuffer)->getUpdater()->setLooping(m_looped);
	}
	else
	{
		alSourcei(m_sourceName->name, AL_LOOPING, m_looped ? AL_TRUE : AL_FALSE);
	}

	alSourcefv(m_sourceName->name, AL_POSITION, m_position);
	alSourcef(m_sourceName->name, AL_GAIN, m_volume);

}
