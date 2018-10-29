#include "C11TimeSystem.h"

#include <chrono>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimeSystem, Mengine::C11TimeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    C11TimeSystem::C11TimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    C11TimeSystem::~C11TimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool C11TimeSystem::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void C11TimeSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t C11TimeSystem::getMilliseconds()
    {
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point::duration epoch = tp.time_since_epoch();

        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

        uint64_t ms64 = (uint64_t)ms.count();

        return ms64;
    }
}
