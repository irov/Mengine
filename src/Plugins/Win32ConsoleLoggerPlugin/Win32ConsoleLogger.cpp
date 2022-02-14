#include "Win32ConsoleLogger.h"

#include "Interface/PlatformInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"

#include <clocale>
#include <iostream>

#include <io.h>
#include <fcntl.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ConsoleLogger::Win32ConsoleLogger()
        : m_createConsole( false )
        , m_CONOUT( nullptr )
        , m_CONERR( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ConsoleLogger::~Win32ConsoleLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ConsoleLogger::_initializeLogger()
    {
        LOGGER_MESSAGE( "initialize windows console" );

        if( this->createConsole_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::_finalizeLogger()
    {
        this->removeConsole_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ConsoleLogger::createConsole_()
    {
        if( ::AllocConsole() == FALSE )
        {
            return false;
        }

        m_createConsole = true;
    
        m_CONOUT = ::freopen( "CONOUT$", "w", stdout );
        m_CONERR = ::freopen( "CONOUT$", "w", stderr );

        uint32_t OPTION_consolex = GET_OPTION_VALUE_UINT32( "consolex", ~0u );
        uint32_t OPTION_consoley = GET_OPTION_VALUE_UINT32( "consoley", ~0u );

        if( OPTION_consolex != ~0u && OPTION_consoley != ~0u )
        {
            HWND hWnd = ::GetConsoleWindow();
            RECT rect;
            ::GetWindowRect( hWnd, &rect );
            ::MoveWindow( hWnd, OPTION_consolex, OPTION_consoley, rect.right - rect.left, rect.bottom - rect.top, TRUE );
        }

        HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );

        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        ::GetConsoleScreenBufferInfo( output_handle, &coninfo );
        coninfo.dwSize.Y = 1000;
        ::SetConsoleScreenBufferSize( output_handle, coninfo.dwSize );

        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();

        std::ios::sync_with_stdio();

        return true;
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

        ::fclose( m_CONERR );
        m_CONERR = nullptr;

        ::FreeConsole();

        m_createConsole = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _filter );

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

        DWORD dWritten;
        ::WriteConsoleA( output_handle, _data, (DWORD)_size, &dWritten, NULL );

        ::SetConsoleTextAttribute( output_handle, consoleInfo.wAttributes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
