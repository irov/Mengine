#	pragma once

#	include "WindowsIncluder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class FPSMonitor
	{
	public:
		FPSMonitor();

	public:
		void initialize();
		void finialize();

	public:
		void setActive( bool _active );
		void setFrameTime( unsigned long _frameTiming );

	public:
		void monitor();

	public:
		DWORD WINAPI threadFrameSignal();

	protected:
		bool m_running;
		bool m_active;

		unsigned long m_frameTiming;

		DWORD  m_threadId;

		HANDLE m_hFrameSignalEvent;
		HANDLE m_hDestroySignalEvent;
		HANDLE m_hFrameSignalThread;
	};
}
