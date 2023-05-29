#include "WatchdogService.h"

#include "Kernel/TimestampHelper.h"

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
    double WatchdogService::watch( const ConstString & _tag )
    {
        Timestamp ms = Helper::getTimestamp();

        MapWatchers::iterator it_found = m_watchers.find( _tag );

        if( it_found == m_watchers.end() )
        {
            m_watchers[_tag] = ms;

            return 0.0;
        }

        uint64_t old_time = it_found->second;

        uint64_t time = ms - old_time;

        it_found->second = ms;

        double sec = (double)(time) * 0.001;

        return sec;
    }
    //////////////////////////////////////////////////////////////////////////
}