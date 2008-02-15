// SoundSystemInterface.cpp : Defines the entry point for the DLL application.
//

#include "ALSoundSystem.h"

//bool gSoundSystemInitialized = false;

#ifdef DEBUG

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#endif

bool    initInterfaceSystem( SoundSystemInterface** _pSoundSystemInterface)
{
	*_pSoundSystemInterface = (SoundSystemInterface*) new ALSoundSystem();
	if(*_pSoundSystemInterface)
		return true;
	return false;
}

void    releaseInterfaceSystem( SoundSystemInterface* _pSoundSystemInterface)
{
	delete static_cast<ALSoundSystem*>(_pSoundSystemInterface);
}