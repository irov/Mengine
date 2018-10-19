#include "WatchdogService.h"

#include "Interface/TimerInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WatchdogService, Mengine::WatchdogService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	WatchdogService::WatchdogService()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	WatchdogService::~WatchdogService()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    float WatchdogService::watch( const String & _tag )
    {
		uint64_t ms = TIMER_SYSTEM()
			->getMilliseconds();

        float sec = (float)(ms) * 0.0001f;

        MapWatchers::iterator it_found = m_watchers.find( _tag );

        if( it_found == m_watchers.end() )
        {
            m_watchers[_tag] = sec;

            return 0.f;
        }

        float old_time = it_found->second;

        float time = sec - old_time;

        it_found->second = sec;

        return time;
    }
}