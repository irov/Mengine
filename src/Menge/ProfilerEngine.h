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
		void displayStats( TextField * _debugTextField );
		void addResourceToProfile(const std::string & _name);
		void removeResourceToProfile(const std::string & _name);
	protected:
		ProfilerSystemInterface * m_interface;
	private:

		struct Stat
		{
			std::string name;
			int numCompiled;
			int numReleased;

			bool operator<(const Stat& rhs) const
			{
				if (this == &rhs)
					return false;

				if (numCompiled >= rhs.numCompiled)
					return true;

				else if (rhs.numCompiled >= numCompiled)
					return false;

				return false;
			}
		};

		std::string m_currentCompiledResource;

		int m_totalCompiled;
		int m_totalReleased;

		typedef std::list<Stat> ProfileResourceList;
		typedef std::map<std::string, Stat*> ProfileResourceMap;
		
		ProfileResourceList m_profileResourceList;
		ProfileResourceMap m_profileResourceMap;
	};
}