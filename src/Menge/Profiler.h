#	pragma once

#	include "Config/Typedef.h"

#	include <map>
#	include <iostream>
#	include <fstream>

#	include "Interface/ApplicationInterface.h"

namespace Menge
{
	struct ProfileBlock
	{
		ProfileBlock()
		{
			currentBlockStartMicroseconds = 0;
			currentCycleTotalMicroseconds = 0;
			lastCycleTotalMicroseconds = 0;
			totalMicroseconds = 0;
		}
		unsigned long int currentBlockStartMicroseconds;
		unsigned long int currentCycleTotalMicroseconds;
		unsigned long int lastCycleTotalMicroseconds;
		unsigned long int totalMicroseconds;
	};

	typedef std::map<Menge::String, ProfileBlock*> TProfileBlockMap;

	class Profiler
	{
	public:
		Profiler();
		~Profiler();

	public:

		enum BlockTimingMethod
		{
			BLOCK_TOTAL_SECONDS,
			BLOCK_TOTAL_MILLISECONDS,
			BLOCK_TOTAL_MICROSECONDS,
			BLOCK_TOTAL_PERCENT,
			BLOCK_CYCLE_SECONDS,
			BLOCK_CYCLE_MILLISECONDS,
			BLOCK_CYCLE_MICROSECONDS,
			BLOCK_CYCLE_PERCENT
		};

		static void init();
		static void destroy();

		static void beginBlock( const Menge::String& _name );
		static void endBlock( const Menge::String& _name );

		static double getBlockTime( const Menge::String& _name, BlockTimingMethod _method );

		static void endProfilingCycle();

		static Menge::String createStatsString(BlockTimingMethod method);

		static void setTimer(TimerInterface * _clock);

		static void setEnabled(bool _enabled);
		static bool isEnabled();

		static void drawStats();

	private:		
		static TProfileBlockMap m_profileBlocks;
		static bool m_enabled;
		static TimerInterface * m_clock;
		static unsigned long int m_currentCycleStartMicroseconds;
		static unsigned long int m_lastCycleDurationMicroseconds;
		static unsigned long int m_cycleNumber;

		static ProfileBlock * findBlock(const Menge::String & _name);
	};
};