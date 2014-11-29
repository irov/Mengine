#   pragma once

#   include "Interface/ThreadSystemInterface.h"

namespace Menge
{
	class ThreadMutexDummy
		: public ThreadMutexInterface
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