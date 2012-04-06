#	include "AlreadyRunningMonitor.h"
#	include "StringConversion.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AlreadyRunningMonitor::AlreadyRunningMonitor( LogServiceInterface * _logService )
		: m_logService(_logService)
		, m_mutex(INVALID_HANDLE_VALUE)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool AlreadyRunningMonitor::run( int _policy, const WString & _windowClassName, const WString & _title )
	{
		// try to create mutex to sure that we are not running already
		WString mutexName = WString(L"MengineMutex_") + _title;
		m_mutex = ::CreateMutex( NULL, FALSE, mutexName.c_str() );
		DWORD error = ::GetLastError();
		// already running

		if( error != ERROR_ALREADY_EXISTS )
		{
			return true;
		}
				 
		if( _policy == EARP_SETFOCUS )
		{
			HWND otherHwnd = ::FindWindowW( _windowClassName.c_str(), _title.c_str() );
			::SetForegroundWindow( otherHwnd );

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
	void AlreadyRunningMonitor::stop()
	{
		if( m_mutex )
		{
			::CloseHandle( m_mutex );
			m_mutex = INVALID_HANDLE_VALUE;
		}
	}
}
