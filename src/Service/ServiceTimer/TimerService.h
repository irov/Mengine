#pragma once

#include "Interface/TimerInterface.h"

#include "Core/ServiceBase.h"

namespace Mengine
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
