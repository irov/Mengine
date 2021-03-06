#   include "SDLTimerSystem.h"

#   include "SDL_timer.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimerSystem, Mengine::SDLTimerSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLTimerSystem::SDLTimerSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLTimerSystem::~SDLTimerSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLTimerSystem::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLTimerSystem::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLTimerSystem::getMilliseconds()
    {
        return (SDL_GetPerformanceCounter() * 1000) / SDL_GetPerformanceFrequency();;
    }
}
