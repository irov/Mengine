#	include "ConsoleLogger.h"

#   include "Interface/UnicodeInterface.h"

#	include <io.h>
#	include <cstdio>
#	include <locale.h>
#	include <fcntl.h>

#   include <iostream>

typedef BOOL (WINAPI *PATTACHCONSOLE)(DWORD);

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConsoleLogger::ConsoleLogger()
		: m_serviceProvider( nullptr )
		, m_verboseLevel( LM_INFO )
		, m_verboseFlag(0xFFFFFFFF)
		, m_createConsole(false)
		, m_ConsoleHandle( nullptr )
	{
		m_fp[0] = nullptr;
		m_fp[1] = nullptr;
		m_fp[2] = nullptr;

		m_lStdHandle[0] = INVALID_HANDLE_VALUE;
		m_lStdHandle[1] = INVALID_HANDLE_VALUE;
		m_lStdHandle[2] = INVALID_HANDLE_VALUE;

		m_hConHandle[0] = 0;
		m_hConHandle[1] = 0;
		m_hConHandle[2] = 0;

		m_hOldHandle[0] = 0;
		m_hOldHandle[1] = 0;
		m_hOldHandle[2] = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	ConsoleLogger::~ConsoleLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ConsoleLogger::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConsoleLogger::initialize()
	{ 
		this->createConsole_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::finalize()
	{
		this->removeConsole_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void ConsoleLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ConsoleLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
	void ConsoleLogger::createConsole_()
	{
		PATTACHCONSOLE pAttachConsole = nullptr;
		
        HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );

		if( hKernel32 != nullptr )
		{
            FARPROC proc = GetProcAddress( hKernel32, "AttachConsole" );

			pAttachConsole = reinterpret_cast<PATTACHCONSOLE>(proc);

			if( pAttachConsole == nullptr )
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
		fgetpos( stdout, &m_pOldHandle[0] );
		m_hOldHandle[0] = _dup( _fileno( stdout ) );
		m_lStdHandle[0] = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConHandle[0] = _open_osfhandle((intptr_t)m_lStdHandle[0], _O_TEXT);
		m_fp[0] = _fdopen( m_hConHandle[0], "w" );
		*stdout = *m_fp[0];
		setvbuf( stdout, NULL, _IONBF, 0 );

		// redirect unbuffered STDIN to the console
		fgetpos( stdin, &m_pOldHandle[1] );
		m_hOldHandle[1] = _dup( _fileno( stdin ) );
		m_lStdHandle[1] = GetStdHandle(STD_INPUT_HANDLE);
		m_hConHandle[1] = _open_osfhandle((intptr_t)m_lStdHandle[1], _O_TEXT);
		m_fp[1] = _fdopen( m_hConHandle[1], "r" );
		*stdin = *m_fp[1];
		setvbuf( stdin, NULL, _IONBF, 0 );
		
		// redirect unbuffered STDERR to the console
		fgetpos( stderr, &m_pOldHandle[2] );
		m_hOldHandle[2] = _dup( _fileno( stderr ) );
		m_lStdHandle[2] = GetStdHandle(STD_ERROR_HANDLE);
		m_hConHandle[2] = _open_osfhandle((intptr_t)m_lStdHandle[2], _O_TEXT);
		m_fp[2] = _fdopen( m_hConHandle[2], "w" );
		*stderr = *m_fp[2];
		setvbuf( stderr, NULL, _IONBF, 0 );
		
		m_ConsoleHandle = m_lStdHandle[2];
		//::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();

		if( hKernel32 != nullptr )
		{
			FreeLibrary( hKernel32 );
		}
		
		std::cout << "console ready.." << std::endl;
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::removeConsole_()
	{ 
		if( m_createConsole == false )
		{
			return;
		}

		fflush( m_fp[0] );
		fflush( m_fp[1] );
		fflush( m_fp[2] );

		fclose( m_fp[0] );
		fclose( m_fp[1] );
		fclose( m_fp[2] );

		_dup2( m_hOldHandle[0], _fileno( stdout ) );
		_dup2( m_hOldHandle[1], _fileno( stdin ) );
		_dup2( m_hOldHandle[2], _fileno( stderr ) );

		_close( m_hOldHandle[0] );
		_close( m_hOldHandle[1] );
		_close( m_hOldHandle[2] );

		FreeConsole();
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count )
	{	
        (void)_flag;

		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		if( ::GetConsoleScreenBufferInfo( m_ConsoleHandle, &consoleInfo ) == FALSE )
		{
			return;
		}

		WORD textColor;
		switch( _level )
		{
		case LM_ERROR:
			textColor = FOREGROUND_RED;
			break;
		case LM_PERFOMANCE:
			textColor = FOREGROUND_BLUE | FOREGROUND_RED;
			break;
		case LM_WARNING:
			textColor = FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case LM_INFO:
			textColor = FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		default:
			textColor = consoleInfo.wAttributes;
			break;
		}

		::SetConsoleTextAttribute( m_ConsoleHandle, textColor);

		std::cout.write( _data, _count );

		::SetConsoleTextAttribute( m_ConsoleHandle, consoleInfo.wAttributes );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConsoleLogger::flush()
	{
		std::cout.flush();
	}
}	// namespace Menge
