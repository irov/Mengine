#include "Win32ConsoleLogger.h"

#include "Interface/LoggerServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"

#include <io.h>
#include <cstdio>
#include <locale.h>
#include <fcntl.h>

#include <iostream>

typedef BOOL( WINAPI *PATTACHCONSOLE )(DWORD);

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ConsoleLogger::Win32ConsoleLogger()
        : m_CONOUT( nullptr )
        , m_createConsole( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ConsoleLogger::~Win32ConsoleLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ConsoleLogger::initialize()
    {
        LOGGER_MESSAGE( "initialize windows console" );

        this->createConsole_();

        LOGGER_SERVICE()
            ->writeHistory( LoggerInterfacePtr( this ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::finalize()
    {
        this->removeConsole_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::createConsole_()
    {
        PATTACHCONSOLE pAttachConsole = nullptr;

        HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );

        if( hKernel32 == nullptr )
        {
            return;
        }

        FARPROC proc = GetProcAddress( hKernel32, "AttachConsole" );

        pAttachConsole = reinterpret_cast<PATTACHCONSOLE>(proc);

        if( pAttachConsole == nullptr )
        {
            FreeLibrary( hKernel32 );
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO coninfo;

        // try to attach to calling console first
        if( pAttachConsole( (DWORD)-1 ) == FALSE )
        {
            // allocate a console for this app
            m_createConsole = (AllocConsole() != FALSE);

            // set the screen buffer to be big enough to let us scroll text
            HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );
            GetConsoleScreenBufferInfo( output_handle, &coninfo );
            coninfo.dwSize.Y = 1000;
            SetConsoleScreenBufferSize( output_handle, coninfo.dwSize );
        }

        m_CONOUT = freopen( "CONOUT$", "w", stdout );

        //::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
        // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
        // point to console as well
        std::ios::sync_with_stdio();

        FreeLibrary( hKernel32 );

        std::cout << "console ready.." << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::removeConsole_()
    {
        if( m_createConsole == false )
        {
            return;
        }

        fclose( m_CONOUT );
        m_CONOUT = nullptr;

        FreeConsole();

        m_createConsole = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::log( ELoggerLevel _level, uint32_t _flag, const Char * _data, uint32_t _count )
    {
        MENGINE_UNUSED( _flag );

        HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );

        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if( ::GetConsoleScreenBufferInfo( output_handle, &consoleInfo ) == FALSE )
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
        case LM_STATISTIC:
            textColor = FOREGROUND_GREEN;
            break;
        case LM_WARNING:
            textColor = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case LM_MESSAGE:
            textColor = FOREGROUND_BLUE | FOREGROUND_RED;
            break;
        case LM_INFO:
            textColor = FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
        default:
            textColor = consoleInfo.wAttributes;
            break;
        }

        ::SetConsoleTextAttribute( output_handle, textColor );

        std::cout.write( _data, _count );

        ::SetConsoleTextAttribute( output_handle, consoleInfo.wAttributes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::flush()
    {
        std::cout.flush();
    }
}
