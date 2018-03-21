#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Core/ServantBase.h"

namespace Mengine
{
	class ThreadMutexDummy
		: public ServantBase<ThreadMutexInterface>
	{
	protected:
		void lock() override 
		{
			//Empty
		};

		void unlock() override 
		{
			//Empty
		};

	protected:
		bool try_lock() override
		{
			//Empty

			return true; 
		};
	};
}