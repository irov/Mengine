#pragma once

#include "WIN32/WindowsIncluder.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class Win32FPSMonitor
	{
	public:
		Win32FPSMonitor();

	public:
		bool initialize();
		void finalize();

	public:
		void setActive( bool _active );
		void setFrameTime( float _frameTiming );

	public:
		void monitor();

	public:
		DWORD WINAPI threadFrameSignal();

	protected:
		bool m_running;
		bool m_active;

		float m_frameTime;

		DWORD  m_threadId;

		HANDLE m_hFrameSignalEvent;
		HANDLE m_hDestroySignalEvent;
		HANDLE m_hFrameSignalThread;
	};
}
