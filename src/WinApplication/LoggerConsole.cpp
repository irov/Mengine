
#	include "LoggerConsole.h"

#	define WIN32_LEAN_AND_MEAN
#	define _WIN32_WINNT 0x0500	// allow AttachConsole
#	include <Windows.h>
#	include <io.h>
#	include <cstdio>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoggerConsole::LoggerConsole()
		: m_createConsole( false )
	{
		int hConHandle;
		HANDLE lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		FILE *fp;
		// try to attach to calling console first
		m_createConsole = ( AttachConsole( ATTACH_PARENT_PROCESS ) == FALSE );
		// allocate a console for this app
		if( m_createConsole == true )
		{
			m_createConsole = ( AllocConsole() == TRUE );
			// set the screen buffer to be big enough to let us scroll text
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
			coninfo.dwSize.Y = 1000;
			SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
		}
		// redirect unbuffered STDOUT to the console
		lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0x4000);
		fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
		setvbuf( stdout, NULL, _IONBF, 0 );
		// redirect unbuffered STDIN to the console
		lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0x4000);
		fp = _fdopen( hConHandle, "r" );
		*stdin = *fp;
		setvbuf( stdin, NULL, _IONBF, 0 );
		// redirect unbuffered STDERR to the console
		lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0x4000);
		fp = _fdopen( hConHandle, "w" );
		*stderr = *fp;
		setvbuf( stderr, NULL, _IONBF, 0 );
		//::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerConsole::~LoggerConsole()
	{
		if( m_createConsole == true )
		{
			FreeConsole();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerConsole::write( const void* _data, std::streamsize _count )
	{
		std::cerr.write( (const char*)_data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerConsole::write( const String& _str )
	{
		std::cerr << _str;
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerConsole::write( int _num )
	{
		std::cerr << _num;
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerConsole::flush()
	{
		// not needed
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge