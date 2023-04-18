#include "Win32ConsoleLogger.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

#include <clocale>
#include <iostream>

#include <io.h>
#include <fcntl.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        BOOL WINAPI ConsoleHandlerRoutine( DWORD dwCtrlType )
        {
            switch( dwCtrlType )
            {
            case CTRL_C_EVENT:
                {
                    LOGGER_MESSAGE_RELEASE( "console close [Ctrl+C]" );

                    return TRUE;
                }break;
            case CTRL_BREAK_EVENT:
                {
                    LOGGER_MESSAGE_RELEASE( "console close [Ctrl+Break]" );

                    return TRUE;
                }break;
            case CTRL_CLOSE_EVENT:
                {
                    LOGGER_MESSAGE_RELEASE( "console close [Ctrl+Close]" );

                    return TRUE;
                }break;
            case CTRL_LOGOFF_EVENT:
                {
                    LOGGER_MESSAGE_RELEASE( "console close [Ctrl+Logoff]" );

                    return TRUE;
                }break;
            case CTRL_SHUTDOWN_EVENT:
                {
                    LOGGER_MESSAGE_RELEASE( "console close [Ctrl+Shutdown]" );

                    return TRUE;
                }break;
            default:
                break;
            }

            return FALSE;
        }
        //////////////////////////////////////////////////////////////////////////
    }
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

        if( ::SetConsoleCtrlHandler( &Detail::ConsoleHandlerRoutine, TRUE ) == FALSE )
        {
            LOGGER_ERROR( "invalid set console ctrl handler: %s"
                , Helper::Win32GetLastErrorMessage()
            );
        }

        m_createConsole = true;

        m_CONOUT = ::freopen( "CONOUT$", "w", stdout );
        m_CONERR = ::freopen( "CONOUT$", "w", stderr );

        uint32_t OPTION_consolex = GET_OPTION_VALUE_UINT32( "consolex", MENGINE_UNKNOWN_SIZE );
        uint32_t OPTION_consoley = GET_OPTION_VALUE_UINT32( "consoley", MENGINE_UNKNOWN_SIZE );

        if( OPTION_consolex != MENGINE_UNKNOWN_SIZE && OPTION_consoley != MENGINE_UNKNOWN_SIZE )
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
    void Win32ConsoleLogger::log( const LoggerMessage & _message )
    {
        HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );

        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if( ::GetConsoleScreenBufferInfo( output_handle, &consoleInfo ) == FALSE )
        {
            return;
        }

        WORD textColor = 0;

        uint32_t color = _message.color;

        if( color & LCOLOR_NONE )
        {
            textColor = consoleInfo.wAttributes;
        }

        if( color & LCOLOR_RED )
        {
            textColor |= FOREGROUND_RED;
        }

        if( color & LCOLOR_GREEN )
        {
            textColor |= FOREGROUND_GREEN;
        }

        if( color & LCOLOR_BLUE )
        {
            textColor |= FOREGROUND_BLUE;
        }

        ::SetConsoleTextAttribute( output_handle, textColor );

        const Char * data = _message.data;
        size_t size = _message.size;

        DWORD dWritten;

        if( _message.flag & ELF_FLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%u]", functionstamp, 0, MENGINE_MAX_PATH );
            ::WriteConsoleA( output_handle, functionstamp, (DWORD)functionstampSize, &dWritten, NULL );
            ::WriteConsoleA( output_handle, " ", 1, &dWritten, NULL );
        }

        if( _message.flag & ELF_FLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            size_t timestampSize = Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            ::WriteConsoleA( output_handle, timestamp, (DWORD)timestampSize, &dWritten, NULL );
            ::WriteConsoleA( output_handle, " ", 1, &dWritten, NULL );
        }

        if( _message.flag & ELF_FLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );
            ::WriteConsoleA( output_handle, threadstamp, (DWORD)threadstampSize, &dWritten, NULL );
            ::WriteConsoleA( output_handle, " ", 1, &dWritten, NULL );
        }

        if( _message.flag & ELF_FLAG_SYMBOL )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            ::WriteConsoleA( output_handle, &symbol, 1, &dWritten, NULL );
            ::WriteConsoleA( output_handle, " ", 1, &dWritten, NULL );
        }

        if( _message.flag & ELF_FLAG_CATEGORY )
        {
            const Char * category_str = _message.category.c_str();
            size_t category_size = _message.category.size();

            ::WriteConsoleA( output_handle, "[", 1, &dWritten, NULL );
            ::WriteConsoleA( output_handle, category_str, (DWORD)category_size, &dWritten, NULL );
            ::WriteConsoleA( output_handle, "]", 1, &dWritten, NULL );
            ::WriteConsoleA( output_handle, " ", 1, &dWritten, NULL );
        }

        ::WriteConsoleA( output_handle, data, (DWORD)size, &dWritten, NULL );
        ::WriteConsoleA( output_handle, "\n", 1, &dWritten, NULL );

        ::SetConsoleTextAttribute( output_handle, consoleInfo.wAttributes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLogger::flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
