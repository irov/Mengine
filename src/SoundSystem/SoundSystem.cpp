#include "SoundSystem.h"

#include "WavSoundBuffer.h"
#include "OggSoundBuffer.h"

#	include "OpenAL/al.h"

bool initInterfaceSystem(SoundSystemInterface** _ptrSoundSystem)
{
	try
	{
		*_ptrSoundSystem = new OpenALSoundSystem();
	}

	catch (...)
	{
		return false;
	}
	
	return true;
}

void releaseInterfaceSystem(SoundSystemInterface* _ptrSoundSystem)
{
	delete static_cast<OpenALSoundSystem*>(_ptrSoundSystem);
}

OpenALSoundSystem::OpenALSoundSystem() 
								: SoundSystemInterface()
{
	mPosition[0] = 0.0f;
	mPosition[1] = 0.0f;
	mPosition[2] = 0.0f;

	mUpDir[0] = 0.0f;
	mUpDir[1] = 0.0f;
	mUpDir[2] = 0.0f;
	mUpDir[3] = 0.0f;
	mUpDir[4] = 0.0f;
	mUpDir[5] = 0.0f;

	mDeviceAL = alcOpenDevice(NULL);

	if (mDeviceAL)
	{
		mContextAL = alcCreateContext(mDeviceAL, NULL);
		alcMakeContextCurrent(mContextAL);
	}

	alListenerfv(AL_VELOCITY, mPosition);
	alListenerfv(AL_POSITION, mPosition);
	alListenerfv(AL_ORIENTATION, mUpDir);
}

OpenALSoundSystem::~OpenALSoundSystem()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(mContextAL);
	alcCloseDevice(mDeviceAL);
}

void	OpenALSoundSystem::setListenerOrient(const float* _position, const float* _updir)
{
	mPosition[0] = _position[0];
	mPosition[1] = _position[1];
	mPosition[2] = _position[2];

	mUpDir[0] = _updir[0];
	mUpDir[1] = _updir[1];
	mUpDir[2] = _updir[2];
	mUpDir[3] = _updir[0];
	mUpDir[4] = _updir[1];
	mUpDir[5] = _updir[2];

	alListenerfv( AL_POSITION, mPosition );
	alListenerfv( AL_ORIENTATION, mUpDir );
}

SoundSourceInterface*	OpenALSoundSystem::loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener)
{
	SoundBufferInterface*	sb = NULL;

	if(_desc.type == WAV)
	{
		sb = new CALWavBuffer(_desc.buffer, _desc.size);
	}

	if(_desc.type == OGG)
	{
		sb = new OggSoundData(_desc.buffer, _desc.size);
	}

	return new OpenALSoundSource(_listener, sb, _desc.isStreamSound);
}

void	OpenALSoundSystem::releaseSoundNode(SoundSourceInterface*	_sn)
{
	delete ((OpenALSoundSource*)_sn);
}