#	pragma once
#	include "Interface/HTTPSystemInterface.h"

namespace Menge
{
    class CURLHTTPUtils
	{
	public:
		//based on javascript encodeURIComponent()
		static HTTPStringType urlencode(const HTTPStringType &c);
	private:
		static HTTPStringType char2hex( char dec );
	};

}