#pragma once

#include "Interface/WatchdogInterface.h"

#include "Config/Typedef.h"

#include "Core/ServiceBase.h"

#include "Config/Map.h"

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
		typedef Map<String, float> TMapWatchers;
		TMapWatchers m_watchers;
	};
}