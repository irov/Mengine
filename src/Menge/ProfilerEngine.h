#	pragma once

#	include "Interface/ProfilerSystemInterface.h"

#	include "Holder.h"

#	include "TextField.h"

#	include <string>

#	include <map>

#	include <set>

namespace Menge
{
	class ProfilerEngine
	{
	public:
		ProfilerEngine( ProfilerSystemInterface * _interface );

	public:
		void beginProfile( const String & _name );
		void endProfile( const String & _name );
		void setEnabled( bool _enabled );
		bool isEnabled() const;
		void displayStats();
		void addResourceToProfile( const String& _name );
		void removeResourceToProfile( const String& _name );
	protected:
		ProfilerSystemInterface * m_interface;
	};
}