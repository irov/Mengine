#	pragma once

#	include "Interface/ThreadSystemInterface.h"

namespace Menge
{
	class PosixThreadSystem
		: public ThreadSystemInterface
	{
	public:
		PosixThreadSystem();
		~PosixThreadSystem();

	public:
		void createThread( ThreadInterface * _thread ) override;

	public:
		void startMutex() override;
		void stopMutex() override;
	};
}