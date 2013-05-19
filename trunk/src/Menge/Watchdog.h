#	pragma once

#   include "Interface/WatchdogInterface.h"

#	include "Config/Typedef.h"

#	include <map>

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

		typedef std::map<String, float> TMapWatchers;
		TMapWatchers m_watchers;
	};
}