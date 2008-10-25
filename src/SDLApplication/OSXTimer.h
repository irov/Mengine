#	pragma once
#	include "Interface/ApplicationInterface.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_DIRECT_H
# include <direct.h>
#endif

namespace Menge
{
	class OSXTimer : public TimerInterface
	{
	public:
		OSXTimer();
	public:
		void reset() override;
		float getDeltaTime() const override;
		unsigned long getMilliseconds() override;
		unsigned long getMicroseconds() override;
		unsigned long getMillisecondsCPU() override;
		unsigned long getMicrosecondsCPU() override;
	private:
		struct timeval start;
		struct timeval last;
		clock_t zeroClock;
	};
};