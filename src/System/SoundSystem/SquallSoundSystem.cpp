#	include "SquallSoundSystem.h"

#	include "squall.h"

#	include <assert.h>

bool initInterfaceSystem(SoundSystemInterface** _ptrSoundSystem)
{
	try
	{
		*_ptrSoundSystem = new SQUALLSoundSystem();
	}

	catch (...)
	{
		return false;
	}

	return true;
}

void releaseInterfaceSystem(SoundSystemInterface* _ptrSoundSystem)
{
	delete static_cast<SQUALLSoundSystem*>(_ptrSoundSystem);
}

SQUALLSoundSystem::SQUALLSoundSystem()
{
	if (SQUALL_Init(0) < 0)
	{
		assert(!"error init squall!");
	}
}

SQUALLSoundSystem::~SQUALLSoundSystem()
{
	SQUALL_Free();
}

void	SQUALLSoundSystem::setListenerOrient(float* _position, float* _front, float* top)
{
	SQUALL_Listener_SetParameters(_position,_position,_front,top);
}

SoundSourceInterface*	SQUALLSoundSystem::createSoundSource(bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface* _listener = 0)
{
	SQUALLSample * squall_sample = (SQUALLSample*)_sample;
	return new SQUALLSoundSource(_isHeadMode,squall_sample,_listener);
}

SoundBufferInterface * SQUALLSoundSystem::createSoundBuffer()
{
	return new SQUALLSample();
}

void SQUALLSoundSystem::releaseSoundBuffer(SoundBufferInterface* _soundBuffer)
{
	delete static_cast<SQUALLSample*>(_soundBuffer);
}

/*SoundSourceInterface*	SQUALLSoundSystem::loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener)
{
	const char * t = _desc.FILENAME_TEMP_FOR_TEST.c_str();
	int sample = SQUALL_Sample_LoadFile((char *)t, _desc.isStreamSound, 0);

	return new SQUALLSoundSource(sample,_desc.isRelativeToListener,_listener);
}
*/
void	SQUALLSoundSystem::releaseSoundNode(SoundSourceInterface* _sn)
{
	delete static_cast<SQUALLSoundSource*>(_sn);
}