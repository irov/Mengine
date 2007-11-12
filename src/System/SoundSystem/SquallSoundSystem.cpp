#	include "SquallSoundSystem.h"

#	include "squall.h"
#	include "windows.h"
#	include <assert.h>
//////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem(SoundSystemInterface* _ptrSoundSystem)
{
	delete static_cast<SQUALLSoundSystem*>(_ptrSoundSystem);
}
//////////////////////////////////////////////////////////////////////////
SQUALLSoundSystem::SQUALLSoundSystem()
{
	if (SQUALL_Init(0) < 0)
	{
		assert(!"error init squall!");
	}
}
//////////////////////////////////////////////////////////////////////////
SQUALLSoundSystem::~SQUALLSoundSystem()
{
	SQUALL_Free();
}
//////////////////////////////////////////////////////////////////////////
void	SQUALLSoundSystem::setListenerOrient(float* _position, float* _front, float* top)
{
	SQUALL_Listener_SetParameters(_position,_position,_front,top);
}
//////////////////////////////////////////////////////////////////////////
SoundSourceInterface*	SQUALLSoundSystem::createSoundSource(bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface* _listener = 0)
{
	SQUALLSample * squall_sample = (SQUALLSample*)_sample;
	return new SQUALLSoundSource(_isHeadMode,squall_sample,_listener);
}
//////////////////////////////////////////////////////////////////////////
SoundBufferInterface * SQUALLSoundSystem::createSoundBufferFromFile( const char* _filename, bool _isStream )
{
	int SampleID = SQUALL_Sample_LoadFile((char*)_filename, !_isStream, 0);

	if( SampleID < 0 )
	{	
		return NULL;
	}

	return new SQUALLSample( SampleID );
}
//////////////////////////////////////////////////////////////////////////
SoundBufferInterface * SQUALLSoundSystem::createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem )
{	
	int SampleID = SQUALL_Sample_LoadFromMemory( _buffer, _size, _newmem, 0 );
	if( SampleID < 0 )
	{	
		return 0;
	}

	return new SQUALLSample( SampleID );
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSoundSystem::releaseSoundBuffer(SoundBufferInterface* _soundBuffer)
{
	delete static_cast<SQUALLSample*>(_soundBuffer);
}
//////////////////////////////////////////////////////////////////////////
void	SQUALLSoundSystem::releaseSoundNode(SoundSourceInterface* _sn)
{
	delete static_cast<SQUALLSoundSource*>(_sn);
}