#	include "LoggerConsole.h"
#	include "StringConversion.h"

#	include <io.h>
#	include <cstdio>

typedef BOOL (WINAPI *PATTACHCONSOLE)(DWORD);

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoggerConsole::LoggerConsole()
		: m_createConsole(false)
	{
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
	void LoggerConsole::createConsole()
	{
		PATTACHCONSOLE pAttachConsole = NULL;
		HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );
		if( hKernel32 != NULL )
		{
			pAttachConsole = reinterpret_cast<PATTACHCONSOLE>( GetProcAddress( hKernel32, "AttachConsole" ) );
			if( pAttachConsole == NULL )
			{
				FreeLibrary( hKernel32 );
				return;
			}
		}

		int hConHandle;
		HANDLE lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		FILE *fp;
	
		// try to attach to calling console first
		if( pAttachConsole( (DWORD)-1 ) == FALSE )
		{
			// allocate a console for this app
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
		m_ConsoleHandle = lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0x4000);
		fp = _fdopen( hConHandle, "w" );
		*stderr = *fp;
		setvbuf( stderr, NULL, _IONBF, 0 );
		//::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();

		if( hKernel32 != NULL )
		{
			FreeLibrary( hKernel32 );
		}

		std::cout << "console ready.." << std::endl;
	}
	//////////////////////////////////////////////////////////////////////////
	void LoggerConsole::log( const void* _data, int _count, EMessageLevel _level )
	{
		std::string ansi;
		
		StringConversion::utf8ToAnsi( String( static_cast<const char*>(_data), _count ), ansi );

		
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		::GetConsoleScreenBufferInfo(m_ConsoleHandle, &consoleInfo);

		WORD textColor;
		switch(_level)
		{
		case LM_ERROR:
			textColor = FOREGROUND_RED;
			break;
		case LM_WARNING:
			textColor = FOREGROUND_RED | FOREGROUND_GREEN;						//yellow
			break;
		case LM_INFO:
			textColor = FOREGROUND_GREEN | FOREGROUND_BLUE;						//light blue
			break;
		default:
			//textColor = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED;	//white
			textColor = consoleInfo.wAttributes;
			break;
		}
		::SetConsoleTextAttribute( m_ConsoleHandle, textColor);


		std::cout.write( ansi.c_str(), ansi.size() );

		//::SetConsoleScreenBufferInfo(m_ConsoleHandle, consoleInfo);
		::SetConsoleTextAttribute(m_ConsoleHandle, consoleInfo.wAttributes);

		//LPDWORD writtenCharsCount;
		//::WriteConsoleA( , ansi.c_str(), ansi.length(), &writtenCharsCount, NULL );
	}
	
}	// namespace Menge
