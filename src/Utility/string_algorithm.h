#	pragma once

#	include <string>

#	include <algorithm>

namespace Utility
{
	inline void string_toupper(std::string &str)
	{
		size_t size = str.size();

		for(size_t i=0;i < size;++i)
		{
			str[i] = toupper(str[i]);
		}
	}

	inline std::string string_from_char(const char *str)
	{
		if( str == 0 )
		{
			return std::string();		
		}

		return std::string(str);			
	}

	inline const std::string & string_empty()
	{
		static std::string empty;

		return empty;
	}
}