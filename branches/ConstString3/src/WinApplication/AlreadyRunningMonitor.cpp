#	include "AlreadyRunningMonitor.h"
#	include "StringConversion.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AlreadyRunningMonitor::AlreadyRunningMonitor( LogSystemInterface * _logSystem )
		: m_logSystem(_logSystem)
		, m_mutex(INVALID_HANDLE_VALUE)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool AlreadyRunningMonitor::run( int _policy, const String & _title )
	{
		// try to create mutex to sure that we are not running already
		WString titleW = StringConversion::utf8ToWChar( _title );
		WString mutexName = WString( MENGE_TEXT("MengeMutex_") ) + titleW;
		m_mutex = ::CreateMutex( NULL, FALSE, mutexName.c_str() );
		DWORD error = ::GetLastError();
		// already running
		if( error == ERROR_ALREADY_EXISTS )
		{
			if( _policy == EARP_SETFOCUS )
			{
				HWND otherHwnd = ::FindWindowW( L"MengeWnd", titleW.c_str() );
				::SetForegroundWindow( otherHwnd );
				return false;
			}
			else if( _policy == EARP_SHOWMESSAGE )
			{
				WString message = WString( MENGE_TEXT("Another instance of ") ) 
					+ titleW 
					+ WString( MENGE_TEXT(" is already running") );

				::MessageBoxW( NULL, message.c_str(), titleW.c_str(), MB_ICONWARNING );
				return false;
			}
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
