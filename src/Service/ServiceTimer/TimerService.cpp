#include "TimerService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimerService, Mengine::TimerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimerService::TimerService()
        : m_prevTime( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimerService::~TimerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TimerService::resetDeltaTime()
    {
        uint64_t currentTime = TIMER_SYSTEM()
            ->getMilliseconds();

        m_prevTime = currentTime;
    }
    //////////////////////////////////////////////////////////////////////////
    float TimerService::getDeltaTime()
    {
        uint64_t currentTime = TIMER_SYSTEM()
            ->getMilliseconds();

        uint64_t deltaTime = currentTime - m_prevTime;

        m_prevTime = currentTime;

        return (float)deltaTime;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t TimerService::getMilliseconds()
    {
        uint64_t milliseconds = TIMER_SYSTEM()
            ->getMilliseconds();

        return milliseconds;
    }
}
