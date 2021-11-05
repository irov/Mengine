#include "Win32ConsoleLogger.h"

#include "Interface/LoggerServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"

#include <clocale>
#include <iostream>

#include <io.h>
#include <fcntl.h>

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
            ->writeHistory( LoggerInterfacePtr::from( this ) );

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
#if !defined(MENGINE_WINDOWS_UNIVERSAL)
        HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );
#else
        HMODULE hKernel32 = ::LoadPackagedLibrary( L"Kernel32.dll", 0 );
#endif

        if( hKernel32 == nullptr )
        {
            return;
        }

        FARPROC proc = ::GetProcAddress( hKernel32, "AttachConsole" );

        typedef BOOL( WINAPI * PATTACHCONSOLE )(DWORD);
        PATTACHCONSOLE pAttachConsole = reinterpret_cast<PATTACHCONSOLE>(proc);

        if( pAttachConsole == nullptr )
        {
            ::FreeLibrary( hKernel32 );

            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO coninfo;

        // try to attach to calling console first
        if( (*pAttachConsole)((DWORD)-1) == FALSE )
        {
            // allocate a console for this app
            m_createConsole = (::AllocConsole() != FALSE);

            // set the screen buffer to be big enough to let us scroll text
            HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );
            ::GetConsoleScreenBufferInfo( output_handle, &coninfo );
            coninfo.dwSize.Y = 1000;
            ::SetConsoleScreenBufferSize( output_handle, coninfo.dwSize );
        }

        uint32_t OPTION_consolex = GET_OPTION_VALUE_UINT32( "consolex", ~0u );
        uint32_t OPTION_consoley = GET_OPTION_VALUE_UINT32( "consoley", ~0u );

        if( OPTION_consolex != ~0u && OPTION_consoley != ~0u )
        {
            HWND hWnd = ::GetConsoleWindow();
            RECT rect;
            ::GetWindowRect( hWnd, &rect );
            ::MoveWindow( hWnd, OPTION_consolex, OPTION_consoley, rect.right - rect.left, rect.bottom - rect.top, TRUE );
        }

        m_CONOUT = ::freopen( "CONOUT$", "w", stdout );

        //::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
        // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
        // point to console as well
        std::ios::sync_with_stdio();

        ::FreeLibrary( hKernel32 );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::removeConsole_()
    {
        if( m_createConsole == false )
        {
            return;
        }

        ::fclose( m_CONOUT );
        m_CONOUT = nullptr;

        ::FreeConsole();

        m_createConsole = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );

        HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );

        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if( ::GetConsoleScreenBufferInfo( output_handle, &consoleInfo ) == FALSE )
        {
            return;
        }

        WORD textColor = 0;

        if( _color & LCOLOR_NONE )
        {
            textColor = consoleInfo.wAttributes;
        }

        if( _color & LCOLOR_RED )
        {
            textColor |= FOREGROUND_RED;
        }

        if( _color & LCOLOR_GREEN )
        {
            textColor |= FOREGROUND_GREEN;
        }

        if( _color & LCOLOR_BLUE )
        {
            textColor |= FOREGROUND_BLUE;
        }

        ::SetConsoleTextAttribute( output_handle, textColor );

        std::cout.write( _data, _size );

        ::SetConsoleTextAttribute( output_handle, consoleInfo.wAttributes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::flush()
    {
        std::cout.flush();
    }
    //////////////////////////////////////////////////////////////////////////
}
