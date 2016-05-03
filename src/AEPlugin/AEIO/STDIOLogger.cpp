#	include "STDIOLogger.h"

#	include <io.h>

#   include <iostream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	STDIOLogger::STDIOLogger()
		: m_serviceProvider( nullptr )
		, m_verboseLevel( LM_INFO )
		, m_verboseFlag( 0xFFFFFFFF )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	STDIOLogger::~STDIOLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void STDIOLogger::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * STDIOLogger::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef BOOL( WINAPI *PATTACHCONSOLE )(DWORD);
	//////////////////////////////////////////////////////////////////////////
	bool STDIOLogger::initialize()
	{ 
		PATTACHCONSOLE pAttachConsole = NULL;
		HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );

		if( hKernel32 != NULL )
		{
			pAttachConsole = reinterpret_cast<PATTACHCONSOLE>(GetProcAddress( hKernel32, "AttachConsole" ));

			if( pAttachConsole == NULL )
			{
				FreeLibrary( hKernel32 );

				return false;
			}
		}

		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		// try to attach to calling console first
		if( pAttachConsole( (DWORD)-1 ) == FALSE )
		{
			// allocate a console for this app
			if( AllocConsole() == FALSE )
			{
				return false;
			}

			// set the screen buffer to be big enough to let us scroll text
			GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &coninfo );
			coninfo.dwSize.Y = 1000;
			SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), coninfo.dwSize );
		}

		// redirect unbuffered STDOUT to the console
		{
			m_consoleOutput.lStdHandle = GetStdHandle( STD_OUTPUT_HANDLE );
			m_consoleOutput.hConHandle = _open_osfhandle( (intptr_t)m_consoleOutput.lStdHandle, 0x4000 );
			m_consoleOutput.fp = _fdopen( m_consoleOutput.hConHandle, "w" );
			*stdout = *m_consoleOutput.fp;
			setvbuf( stdout, NULL, _IONBF, 0 );
		}

		// redirect unbuffered STDIN to the console
		{
			m_consoleInput.lStdHandle = GetStdHandle( STD_INPUT_HANDLE );
			m_consoleInput.hConHandle = _open_osfhandle( (intptr_t)m_consoleInput.lStdHandle, 0x4000 );
			m_consoleInput.fp = _fdopen( m_consoleInput.hConHandle, "r" );
			*stdin = *m_consoleInput.fp;
			setvbuf( stdin, NULL, _IONBF, 0 );
		}

		// redirect unbuffered STDERR to the console
		{
			m_consoleError.lStdHandle = GetStdHandle( STD_ERROR_HANDLE );
			m_consoleError.hConHandle = _open_osfhandle( (intptr_t)m_consoleError.lStdHandle, 0x4000 );
			m_consoleError.fp = _fdopen( m_consoleError.hConHandle, "w" );
			*stderr = *m_consoleError.fp;
			setvbuf( stderr, NULL, _IONBF, 0 );
		}

		//m_ConsoleHandle = lStdHandle[2];
		//::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();

		if( hKernel32 != NULL )
		{
			FreeLibrary( hKernel32 );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void STDIOLogger::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void STDIOLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void STDIOLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool STDIOLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
	void STDIOLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count )
	{	
		(void)_level;
        (void)_flag;

		std::cout.write( _data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void STDIOLogger::flush()
	{
		std::cout.flush();
	}
}	// namespace Menge
