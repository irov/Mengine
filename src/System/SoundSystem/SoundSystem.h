#pragma once

#	include "SoundSource.h"

#	include "OpenAL/alc.h"


class	OpenALSoundSystem 
	: public SoundSystemInterface
{
public:
	OpenALSoundSystem();
	~OpenALSoundSystem();

public:
	void					releaseSoundNode(SoundSourceInterface*	_sn);
	void					setListenerOrient(const float * _position, const float * _updir);
	SoundSourceInterface*	loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface* _listener = 0);

private:
	ALCdevice* m_deviceAL;
	ALCcontext* m_contextAL;
	float m_pos[3];
	float m_updir[6];
};