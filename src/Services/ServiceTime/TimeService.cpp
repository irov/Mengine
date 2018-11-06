#include "TimeService.h"

#include "Interface/TimeSystemInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimeService, Mengine::TimeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimeService::TimeService()
        : m_prevTime( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimeService::~TimeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TimeService::resetDeltaTime()
    {
        uint64_t currentTime = TIME_SYSTEM()
            ->getMilliseconds();

        m_prevTime = currentTime;
    }
    //////////////////////////////////////////////////////////////////////////
    float TimeService::getDeltaTime()
    {
        uint64_t currentTime = TIME_SYSTEM()
            ->getMilliseconds();

        uint64_t deltaTime = currentTime - m_prevTime;

        m_prevTime = currentTime;

        return (float)deltaTime;
    }
}
