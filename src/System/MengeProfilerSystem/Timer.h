#	pragma once

#	include "windows.h"
#	include <ctime>

class Timer
{
private:
	clock_t mZeroClock;

	DWORD mStartTick;
	LONGLONG mLastTime;
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mFrequency;

	DWORD mTimerMask;
public:
	
	Timer();
	~Timer();

	void reset();

	unsigned long getMilliseconds();
	unsigned long getMicroseconds();
	unsigned long getMillisecondsCPU();
	unsigned long getMicrosecondsCPU();
};