#include "FSLSoundSource.h"
#include <assert.h>

FSLSound::FSLSound(void* _data, int _size, SoundNodeListenerInterface* _listener, bool _isStreaming, const char* strFileFormat)
{
	if(_isStreaming)
	{
		obj = fslStreamSound(_data,_size);
	}
	else
	{
		obj = fslLoadSoundFromData(strFileFormat,_data,_size);
	}
	if (_listener)
	{

		struct PauseCallback : public Callback 
		{
			SoundNodeListenerInterface* listener;
			PauseCallback(SoundNodeListenerInterface* _listener):listener(_listener){};
			void	call()
			{
				listener->listenPaused();
			};
		};

		struct StopCallback : public Callback 
		{
			SoundNodeListenerInterface* listener;
			StopCallback(SoundNodeListenerInterface* _listener):listener(_listener){};
			void	call()
			{
				listener->listenStopped();
			};
		};

		fslSoundCallbackPause(obj, new PauseCallback(_listener));
		fslSoundCallbackStop(obj, new StopCallback(_listener));
	}
}

FSLSound::~FSLSound()
{
	fslFreeSound(obj);
}

bool FSLSound::play()
{
	fslSoundPlay(obj);
	return true;
}
bool FSLSound::pause()
{
	fslSoundPause(obj);
	return true;
}
bool FSLSound::stop()
{
	fslSoundStop(obj);
	return true;
}

bool FSLSound::isPlaying() const
{
	return fslSoundIsPlaying(obj);
}

bool FSLSound::isPaused() const
{
	return fslSoundIsPaused(obj);
}

bool FSLSound::isStopped() const
{
	return fslSoundIsStopped(obj);
}

void FSLSound::setPitch(float pitch)
{
	fslSoundSetPitch(obj,pitch);
}

float FSLSound::getPitch() const
{
	return fslSoundGetPitch(obj);
}

void FSLSound::setGain(float gain)
{
	fslSoundSetGain(obj,gain);
}

float FSLSound::getGain() const
{
	return fslSoundGetGain(obj);
}

void FSLSound::setRelativeToListener(bool relative)
{
	fslSoundSetSourceRelative(obj,relative);
}

bool FSLSound::isRelativeToListener() const
{
	return fslSoundIsSourceRelative(obj);
}

void FSLSound::setPosition(float x, float y, float z)
{
	fslSoundSetPosition(obj,x,y,z);
}
const float* FSLSound::getPosition() const
{
	//float * pos[3];
	//fslSoundGetPosition(obj,*pos[0],*pos[1],*pos[2]);
	assert(0);
	return 0;
}
void FSLSound::setDirection(float x, float y, float z)
{
assert(0);
}
const float* FSLSound::getDirection() const
{
	assert(0);
	return 0;
}
void FSLSound::setLoop(bool loop)
{
	fslSoundSetLooping(obj,loop);
}

bool FSLSound::isLooping() const
{
	return fslSoundIsLooping(obj);
}

double	FSLSound::getSizeSec()
{
	return (float)fslSoundGetBufferLength(obj) / 1000.0f;
}