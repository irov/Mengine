#	pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0410	// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#	include <Windows.h>

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