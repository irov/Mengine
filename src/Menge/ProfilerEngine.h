#	pragma once

#	include "Interface/ProfilerSystemInterface.h"

#	include "Holder.h"

#	include "TextField.h"

#	include <string>

#	include <map>

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
		void displayStats( TextField * _debugTextField );
	protected:
		ProfilerSystemInterface * m_interface;

	};
}