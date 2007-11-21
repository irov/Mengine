
#include "stdafx.h"

#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

void CALLBACK ListenStoppedCallback(void* _listener, BOOL)
{
	if(_listener)
		static_cast<SoundNodeListenerInterface*>(_listener)->listenStopped();
}

ALSoundSource::ALSoundSource(ALSoundSystem* _soundSystem) :
mSoundBuffer(NULL),
mListener(NULL),
mSourceName(AL_INVALID),
m_soundSystem(_soundSystem)
{
	//alGenSources(1, &mSourceName);

	ZeroMemory(mPosition, sizeof(float)*3);
}

ALSoundSource::~ALSoundSource()
{
	if(isPlaying())
	{
		stop();
	}
	//alDeleteSources(1, &mSourceName);
}

void ALSoundSource::play()
{
	mSourceName = m_soundSystem->getFreeSource();

	if(mSoundBuffer && mSoundBuffer->isStreamed())
	{
		alSourcei(mSourceName, AL_BUFFER, NULL);
	    alSourcei(mSourceName, AL_LOOPING, AL_FALSE); //Streaming sources can't loop
		static_cast<ALSoundBufferStream*>(mSoundBuffer)->record(mSourceName);
		Sleep(100);	// wait for starting thread for streamed buffer
	}
	else
		alSourcei(mSourceName, AL_BUFFER, mSoundBuffer->getBufferName());

	alSourcePlay(mSourceName);

	if(ALenum error = alGetError())
	{
		printf("ALSoundSource::play() ERROR!  -  %d, %d", error, mSourceName);
	}

	SetTimerQueueTimer(NULL, (WAITORTIMERCALLBACK)ListenStoppedCallback, mListener, getLengthMs(), 0, 0);
 
}

void ALSoundSource::pause()
{
	alSourcePause(mSourceName);
	if(mListener)
		mListener->listenPaused(true);
}

void ALSoundSource::stop()
{
	if(mSoundBuffer && mSoundBuffer->isStreamed())
		static_cast<ALSoundBufferStream*>(mSoundBuffer)->stop();

	alSourceStop(mSourceName);

	if(mListener)
		mListener->listenStopped();
}

bool ALSoundSource::isPlaying() const
{
  ALint state;
  alGetSourceiv(mSourceName,AL_SOURCE_STATE,&state);
  if(state == AL_PLAYING)
	  return true;
  return false;
}

void ALSoundSource::setVolume( float _volume )
{
	alSourcef(mSourceName, AL_GAIN, _volume);
}

float ALSoundSource::getVolume() const
{
	ALfloat gain;
	alGetSourcefv(mSourceName, AL_GAIN, &gain);
	return gain;
}

void ALSoundSource::setPosition( float _x, float _y, float _z )
{
	alSource3f(mSourceName, AL_POSITION, _x, _y, _z);
	mPosition[0] = _x;
	mPosition[1] = _y;
	mPosition[2] = _z;
}

const float * ALSoundSource::getPosition() const
{
	return mPosition;
}

void ALSoundSource::setLooped( bool _loop )
{
	if(mSoundBuffer && mSoundBuffer->isStreamed())
		static_cast<ALSoundBufferStream*>(mSoundBuffer)->getUpdater()->setLooping(_loop);
	else
		alSourcei(mSourceName, AL_LOOPING, _loop ? AL_TRUE : AL_FALSE);
}

bool ALSoundSource::isLooping() const
{
	if(mSoundBuffer && mSoundBuffer->isStreamed())
		return static_cast<ALSoundBufferStream*>(mSoundBuffer)->getUpdater()->isLooping();
	// else
	ALint looping;
	alGetSourceiv(mSourceName, AL_LOOPING, &looping);
	return (looping == AL_TRUE);
}

int ALSoundSource::getLengthMs()
{
	if(mSoundBuffer)
		return mSoundBuffer->getLenghtMs();
	return 0;
}

int ALSoundSource::getPosMs()
{
	ALint pos;
	alGetSourcei(mSourceName, AL_SEC_OFFSET, &pos);
	return pos;
}

void ALSoundSource::setSoundBuffer( ALSoundBuffer* _soundBuffer)
{
	mSoundBuffer = _soundBuffer;
	/*if(!_soundBuffer->isStreamed())
		alSourcei(mSourceName, AL_BUFFER, mSoundBuffer->getBufferName() );
	else
		alSourcei(mSourceName, AL_BUFFER, NULL);*/
}

void ALSoundSource::setAmbient(bool _ambient)
{
	if(_ambient)
	{
		alSourcei(mSourceName, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(mSourceName, AL_ROLLOFF_FACTOR, 0.0f);
		alSource3f(mSourceName, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
	} 
	else 
	{
		alSourcei(mSourceName, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(mSourceName, AL_ROLLOFF_FACTOR, 1.0f);
	}

}
