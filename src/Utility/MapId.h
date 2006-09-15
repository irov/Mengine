#	pragma once

#	include <map>
#	include <string>

#	include "Enumerator.h"

namespace Utility
{
	template<int N>
	class MapId
	{
		typedef std::map<std::string, size_t> TMapId;

	public:
		static size_t GetId(const std::string &Name)
		{
			TMapId &Map = GetMapId();
			
			TMapId::iterator it_find = Map.find(Name);

			if( it_find == Map.end() )
			{
				Map.insert(std::make_pair(Name,0));

				return 0;
			}

			++it_find->second;

			return it_find->second;
		}

	private:
		static TMapId & GetMapId()
		{
			static TMapId Map;
			return Map;
		}
	};
}
