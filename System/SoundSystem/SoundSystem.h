#pragma once

#include "SoundSource.h"

class	OpenALSoundSystem : public SoundSystemInterface
{
public:
	OpenALSoundSystem();
	~OpenALSoundSystem();			
	void					releaseSoundNode(SoundSourceInterface*	_sn);
	void					setListenerOrient(const float * _position, const float * _updir);
	SoundSourceInterface*	loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface* _listener = NULL);
private:
	ALCdevice*				mDeviceAL;
	ALCcontext*				mContextAL;
	float					mPosition[3];
	float					mUpDir[6];
};