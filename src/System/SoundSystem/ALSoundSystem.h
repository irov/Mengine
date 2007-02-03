#	pragma once

#	include "interfaces.h"
#	include "OpenAL/alc.h"
#	include "OpenAL/al.h"
#	include	"OpenAL/alut.h"

struct ALSoundInfo
{
	ALenum	_format;	
	ALsizei _frequency;
	ALsizei _size;
	ALvoid* _data;
};

class	ALSoundSystem 
	: public SoundSystemInterface
{
public:
	ALSoundSystem();
	~ALSoundSystem();

public:
	void				setListenerOrient(const float* _position, const float* _updir);
	SoundSourceInterface*		loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener = 0);
	void				releaseSoundNode(SoundSourceInterface* _sn);
public:
	static void checkError();
private:
	ALCdevice* m_deviceAL;
	ALCcontext* m_contextAL;
	float m_pos[3];
	float m_updir[6];
};