#	include "ConsoleLogger.h"

#   include "Interface/UnicodeInterface.h"

#	include <io.h>
#	include <cstdio>

typedef BOOL (WINAPI *PATTACHCONSOLE)(DWORD);

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConsoleLogger::ConsoleLogger( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
        , m_createConsole(false)
		, m_verboseLevel(LM_INFO)
        , m_verboseFlag(0xFFFFFFFF)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConsoleLogger::~ConsoleLogger()
	{
		if( m_createConsole == false )
		{
			return;
		}

		//_close( hConHandle[0] );
		//_close( hConHandle[1] );
		//_close( hConHandle[2] );
		
		fclose( fp[0] );
		fclose( fp[1] );
		fclose( fp[2] );

		FreeConsole();
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void ConsoleLogger::setVerboseFlag( size_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ConsoleLogger::validMessage( EMessageLevel _level, size_t _flag ) const
	{
		if( m_verboseLevel < _level )
		{
			return false;
		}

        if( _flag == 0 )
        {
            return true;
        }

        if( (m_verboseFlag & _flag) == 0 )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::createConsole()
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

		
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
	
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
		lStdHandle[0] = GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle[0] = _open_osfhandle((intptr_t)lStdHandle[0], 0x4000);
		fp[0] = _fdopen( hConHandle[0], "w" );
		*stdout = *fp[0];
		setvbuf( stdout, NULL, _IONBF, 0 );

		// redirect unbuffered STDIN to the console
		lStdHandle[1] = GetStdHandle(STD_INPUT_HANDLE);
		hConHandle[1] = _open_osfhandle((intptr_t)lStdHandle[1], 0x4000);
		fp[1] = _fdopen( hConHandle[1], "r" );
		*stdin = *fp[1];
		setvbuf( stdin, NULL, _IONBF, 0 );
		
		// redirect unbuffered STDERR to the console
		lStdHandle[2] = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle[2] = _open_osfhandle((intptr_t)lStdHandle[2], 0x4000);
		fp[2] = _fdopen( hConHandle[2], "w" );
		*stderr = *fp[2];
		setvbuf( stderr, NULL, _IONBF, 0 );
		
		m_ConsoleHandle = lStdHandle[2];
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
	void ConsoleLogger::log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count )
	{	
        (void)_flag;

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

		std::cout.write( _data, _count );

		//::SetConsoleScreenBufferInfo(m_ConsoleHandle, consoleInfo);
		::SetConsoleTextAttribute(m_ConsoleHandle, consoleInfo.wAttributes);

		//LPDWORD writtenCharsCount;
		//::WriteConsoleA( , ansi.c_str(), ansi.length(), &writtenCharsCount, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::flush()
	{

	}
}	// namespace Menge
