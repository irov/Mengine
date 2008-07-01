#	include "Utils.h"
#	include <assert.h>
#	include <algorithm>
#	include "pugixml.hpp"

bool Utility::useHalfTexel = false;
int Utility::JPEGQuality = 95;

const char* Utility::convert_number_to_string(double value)
{
	static char buf[100];

	sprintf(buf, "%.12f", value);

	if (strchr(buf, '.'))
	{
		char* ptr = buf + strlen(buf) - 1;
		for (; *ptr == '0'; --ptr) ;
		*(ptr+1) = 0;
	}

	return buf;
}