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
		float watch( const String & _tag ) override;
        void destroy() override;

	protected:
		typedef std::map<String, float> TMapWatchers;
		TMapWatchers m_watchers;
	};
}