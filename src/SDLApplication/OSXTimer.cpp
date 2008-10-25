#	include "OSXTimer.h"

namespace Menge
{
	OSXTimer::OSXTimer()
	{
		reset();
	}

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
		double dt = (now.tv_sec-mLastDeltaTimer.tv_sec)*1000+(now.tv_usec-last.tv_usec)/1000;
		last = time;

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