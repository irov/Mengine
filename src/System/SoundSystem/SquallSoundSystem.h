#	pragma once

#	include "SquallSoundSource.h"

class SQUALLSoundSystem
	: public SoundSystemInterface
{
public:
	SQUALLSoundSystem();
	~SQUALLSoundSystem();

public:
	void	setListenerOrient(float* _position, float* _front, float* top);
	SoundSourceInterface *	createSoundSource(bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface*	_listener);
	SoundBufferInterface * createSoundBuffer();
	void releaseSoundBuffer(SoundBufferInterface* _soundBuffer);
	void	releaseSoundNode(SoundSourceInterface* _sn);
};

bool	initInterfaceSystem(SoundSystemInterface** );
void	releaseInterfaceSystem(SoundSystemInterface* );
