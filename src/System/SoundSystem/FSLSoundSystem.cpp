#include "FSLSoundSystem.h"


bool initInterfaceSystem(SoundSystemInterface** _ptrSoundSystem)
{
	try
	{
		*_ptrSoundSystem = new FSLSoundSystem();
	}

	catch (...)
	{
		return false;
	}

	return true;
}

void releaseInterfaceSystem(SoundSystemInterface* _ptrSoundSystem)
{
	delete static_cast<FSLSoundSystem*>(_ptrSoundSystem);
}

FSLSoundSystem::FSLSoundSystem() 
: SoundSystemInterface()
{
	fslInit(FSL_SS_ALUT);
	fslSetAutoUpdate(true);
	fslSetListenerVelocity(0,0,0);
	fslSetListenerPosition(0,0,0);
	fslSetListenerOrientation(0,0,0,0,0,0);
}

FSLSoundSystem::~FSLSoundSystem()
{
	fslShutDown();
}

void	FSLSoundSystem::setListenerOrient(const float* _position, const float* _updir)
{
	fslSetListenerPosition(_position[0], _position[1], _position[2]);
	fslSetListenerOrientation(_updir[0], _updir[1], _updir[2],
		_updir[3], _updir[4], _updir[5]);
}

SoundSourceInterface*	FSLSoundSystem::loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener)
{
	if(_desc.type == SoundDataDesc::WAV)
		return new FSLSound(_desc.buffer,_desc.size,_listener,_desc.isStreamSound,"wav");

	if(_desc.type == SoundDataDesc::OGG)
		return new FSLSound(_desc.buffer,_desc.size,_listener,_desc.isStreamSound,"ogg");

	return 0;
}

void				FSLSoundSystem::releaseSoundNode(SoundSourceInterface* _sn)
{
	delete _sn;
}
