#	pragma once

#	include "Interface/TimerInterface.h"

#   include <wtypes.h>

namespace Menge
{
	class Win32TimerSystem 
		: public ServiceBase<TimerSystemInterface>
	{
	public:
		Win32TimerSystem();
		~Win32TimerSystem();

	public:
        bool _initialize() override;
		void _finalize() override;

	public:
		uint64_t getMilliseconds() override;

	protected:
		DWORD_PTR m_processAffinityMask;

		LARGE_INTEGER m_startTime;
		LARGE_INTEGER m_frequency;
	};
};
