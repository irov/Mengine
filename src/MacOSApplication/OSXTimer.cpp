#	include "OSXTimer.h"

#ifdef MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
/*	static time_t gtc_base = 0;
	static DWORD gtc_last = 0;
	static time_t last_sec = 0;
	static unsigned int last_msec = 0;
	static bool bt_last = false;

	static void gettimeofday_calibrate (void)
	{
		const time_t t = time(NULL);
		const DWORD gtc = GetTickCount();
		gtc_base = t - gtc/1000;
		gtc_last = gtc;
	}

	int gettimeofday (struct timeval *tv, void *tz)
	{
		const DWORD gtc = GetTickCount();
		bool bt = false;
		time_t sec;
		unsigned int msec;
		const int backtrack_hold_seconds = 10;

		// recalibrate at the dreaded 49.7 day mark
		if (!gtc_base || gtc < gtc_last)
			gettimeofday_calibrate ();
		gtc_last = gtc;

		sec = gtc_base + gtc / 1000;
		msec = gtc % 1000;

		if (sec == last_sec)
		{
			if (msec < last_msec)
			{
				msec = last_msec;
				bt = true;
			}
		}
		else if (sec < last_sec)
		{
			if (sec > last_sec - backtrack_hold_seconds)
			{
				sec = last_sec;
				msec = last_msec;
			}
			bt = true;
		}

		tv->tv_sec = last_sec = sec;
		tv->tv_usec = (last_msec = msec) * 1000;

		if (bt && !bt_last)
		{
			gettimeofday_calibrate ();
		}
		bt_last = bt;

		return 0;
	}*/
#endif

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OSXTimer::OSXTimer()
	{
		reset();
	}
	//////////////////////////////////////////////////////////////////////////
	void OSXTimer::reset()
	{
		zeroClock = clock();
		gettimeofday(&start, NULL); 
	}
	//////////////////////////////////////////////////////////////////////////
	float OSXTimer::getDeltaTime() const
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		double dt = (now.tv_sec-last.tv_sec)*1000+(now.tv_usec-last.tv_usec)/1000;
		last = now;

		return dt;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long OSXTimer::getMilliseconds()
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		return (now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000; 
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long OSXTimer::getMicroseconds()
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		return (now.tv_sec-start.tv_sec)*1000000+(now.tv_usec-start.tv_usec); 
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long OSXTimer::getMillisecondsCPU()
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long OSXTimer::getMicrosecondsCPU()
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
}