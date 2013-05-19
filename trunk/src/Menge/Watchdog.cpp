#	include "Watchdog.h"

#	include <ctime>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Watchdog, Menge::WatchdogInterface, Menge::Watchdog );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Watchdog::Watchdog()
        : m_serviceProvider(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Watchdog::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Watchdog::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	float Watchdog::watch( const String & _tag )
	{
		clock_t cl = clock();
		float sec = (float)(cl) / float(CLOCKS_PER_SEC);

		TMapWatchers::iterator it_found = m_watchers.find( _tag );
		
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