#pragma once

#   include "Interface/WatchdogInterface.h"

#include "Config/Typedef.h"

#   include "Core/ServiceBase.h"

#include "stdex/stl_map.h"

namespace Mengine
{
	class Watchdog
        : public ServiceBase<WatchdogInterface>
	{
	public:
		Watchdog();

	public:
		float watch( const String & _tag ) override;

	protected:
		typedef stdex::map<String, float> TMapWatchers;
		TMapWatchers m_watchers;
	};
}