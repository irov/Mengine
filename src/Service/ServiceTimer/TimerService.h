#	pragma once

#	include "Interface/TimerInterface.h"

namespace Menge
{
	class TimerService
		: public ServiceBase<TimerServiceInterface>
	{
	public:
		TimerService();
		~TimerService();

	public:
		void resetDeltaTime() override;
		float getDeltaTime() override;
		
	public:
		uint64_t getMilliseconds() override;

	protected:
		uint64_t m_prevTime;
	};
};
