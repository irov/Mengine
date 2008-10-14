#	include "SDLTimer.h"
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
	void gettimeofday(struct timeval* p, void* tz /* IGNORED */){
		union {
			long long ns100; /*time since 1 Jan 1601 in 100ns units */
			FILETIME ft;
		} _now;

		GetSystemTimeAsFileTime( &(_now.ft) );
		p->tv_usec=(long)((_now.ns100 / 10LL) % 1000000LL );
		p->tv_sec= (long)((_now.ns100-(116444736000000000LL))/10000000LL);
		return;
	}
#endif


	void SDLTimer::reset()
	{
		gettimeofday(&start, NULL);
	}

	unsigned long SDLTimer::getMilliseconds()
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		return (now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;
	}

	unsigned long SDLTimer::getMicroseconds()
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		return (now.tv_sec-start.tv_sec)*1000000+(now.tv_usec-start.tv_usec);
	}	