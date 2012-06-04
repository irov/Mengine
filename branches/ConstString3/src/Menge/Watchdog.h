#	pragma once

#	include "Core/Holder.h"

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	class Watchdog
		: public Holder<Watchdog>
	{
	public:
		Watchdog();

	public:
		float watch( const String & _tag );

	protected:
		typedef std::map<String, float> TMapWatchers;
		TMapWatchers m_watchers;
	};
}