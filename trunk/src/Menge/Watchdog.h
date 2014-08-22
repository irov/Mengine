#	pragma once

#   include "Interface/WatchdogInterface.h"

#	include "Config/Typedef.h"

#	include <stdex/stl_map.h>

namespace Menge
{
	class Watchdog
        : public WatchdogInterface
	{
	public:
		Watchdog();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		float watch( const String & _tag ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef stdex::map<String, float> TMapWatchers;
		TMapWatchers m_watchers;
	};
}