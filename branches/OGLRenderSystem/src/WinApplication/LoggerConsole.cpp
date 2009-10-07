
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
	void LoggerConsole::write( const void* _data, int _count )
	{
		std::string ansi;
		utf8_to_ansi_( static_cast<const char*>( _data ), _count, &ansi );
		std::cerr.write( ansi.c_str(), ansi.size() );
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerConsole::flush()
	{
		// not needed
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerConsole::utf8_to_ansi_( const char* _utf8, int _size, std::string* _ansiString )
	{
		//int wide_size = MultiByteToWideChar( CP_UTF8, 0, _utf8, -1, NULL, 0 );
		//m_wBuffer.resize( wide_size );
		//MultiByteToWideChar( CP_UTF8, 0, _utf8, -1, (&m_wBuffer[0]), wide_size );
		//int ansi_size = WideCharToMultiByte( CP_ACP, 0, (&m_wBuffer[0]), wide_size, NULL, 0, NULL, NULL );
		//m_aBuffer.resize( ansi_size );
		//WideCharToMultiByte( CP_ACP, 0, (&m_wBuffer[0]), wide_size, (&m_aBuffer[0]), ansi_size, NULL, NULL );
		//_ansiString->assign( /*_utf8*/ &(m_aBuffer[0]) ); 
		// utf8 to ansi
		int wide_size = MultiByteToWideChar( CP_UTF8, 0, _utf8, -1, NULL, 0 );
		wchar_t* wide = new wchar_t[wide_size];
		MultiByteToWideChar( CP_UTF8, 0, _utf8, -1, wide, wide_size );
		int anis_size = WideCharToMultiByte( CP_ACP, 0, wide, wide_size, NULL, 0, NULL, NULL );
		char* ansi = new char[anis_size];
		WideCharToMultiByte( CP_ACP, 0, wide, wide_size, ansi, anis_size, NULL, NULL );
		_ansiString->assign( ansi );
		delete[] wide;
		delete[] ansi;
		
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
