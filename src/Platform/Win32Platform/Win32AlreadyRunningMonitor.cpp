#include "Win32AlreadyRunningMonitor.h"

#include "Kernel/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Win32AlreadyRunningMonitor::Win32AlreadyRunningMonitor()
		: m_mutex(INVALID_HANDLE_VALUE)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32AlreadyRunningMonitor::~Win32AlreadyRunningMonitor()
	{
		this->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32AlreadyRunningMonitor::run( int _policy, const WChar * _windowClassName, const WString & _title )
	{
		// try to create mutex to sure that we are not running already
		WString mutexName = L"MengineAlreadyRunningMonitorMutex_";
		mutexName += _title;

		m_mutex = ::CreateMutex( NULL, FALSE, mutexName.c_str() );
		
		DWORD error = ::GetLastError();
		// already running

		if( error != ERROR_ALREADY_EXISTS )
		{
			return true;
		}
				 
		if( _policy == EARP_SETFOCUS )
		{
			HWND otherHwnd = ::FindWindow( _windowClassName, _title.c_str() );
			::SetForegroundWindow( otherHwnd );

			LOGGER_ERROR("AlreadyRunningMonitor FOCUS to other instance of engine"
				);

			return false;
		}

		if( _policy == EARP_SHOWMESSAGE )
		{
			WString message = WString(L"Another instance of ") 
				+ _title 
				+ WString(L" is already running");

			::MessageBoxW( NULL, message.c_str(), _title.c_str(), MB_ICONWARNING );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32AlreadyRunningMonitor::stop()
	{
		if( m_mutex != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_mutex );
			m_mutex = INVALID_HANDLE_VALUE;
		}
	}
}
