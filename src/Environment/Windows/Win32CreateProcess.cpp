#include "Win32CreateProcess.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/LoggerMessage.h"
#include "Kernel/UnicodeHelper.h"

#include "Config/StdString.h"
#include "Config/Path.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static bool appendCommandLineCharacters( WChar * const _command, size_t _capacity, size_t * const _size, WChar _character, size_t _count )
            {
                if( *_size >= _capacity )
                {
                    return false;
                }

                if( _count >= _capacity - *_size )
                {
                    return false;
                }

                for( size_t index = 0; index != _count; ++index )
                {
                    _command[(*_size)++] = _character;
                }

                _command[*_size] = L'\0';

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool appendCommandLineArgument( const WChar * _argument, WChar * const _command, size_t _capacity, size_t * const _size )
            {
                if( *_size != 0 )
                {
                    if( Detail::appendCommandLineCharacters( _command, _capacity, _size, L' ', 1 ) == false )
                    {
                        return false;
                    }
                }

                if( Detail::appendCommandLineCharacters( _command, _capacity, _size, L'"', 1 ) == false )
                {
                    return false;
                }

                size_t backslashes = 0;

                for( const WChar * character = _argument; *character != L'\0'; ++character )
                {
                    if( *character == L'\\' )
                    {
                        ++backslashes;

                        continue;
                    }

                    size_t backslashCount = backslashes;

                    if( *character == L'"' )
                    {
                        backslashCount = backslashes * 2 + 1;
                    }

                    if( Detail::appendCommandLineCharacters( _command, _capacity, _size, L'\\', backslashCount ) == false )
                    {
                        return false;
                    }

                    if( Detail::appendCommandLineCharacters( _command, _capacity, _size, *character, 1 ) == false )
                    {
                        return false;
                    }

                    backslashes = 0;
                }

                size_t trailingBackslashCount = backslashes * 2;

                if( Detail::appendCommandLineCharacters( _command, _capacity, _size, L'\\', trailingBackslashCount ) == false )
                {
                    return false;
                }

                if( Detail::appendCommandLineCharacters( _command, _capacity, _size, L'"', 1 ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32CreateProcess( const Char * _executable, const ArgumentStrings & _arguments, uint32_t * const _exitCode )
        {
            WPath executable = {L'\0'};

            if( Helper::utf8ToUnicode( _executable, executable, MENGINE_MAX_PATH ) == false )
            {
                LOGGER_ERROR( "unable to convert process executable to Unicode" );

                return false;
            }

            WChar command[MENGINE_MAX_COMMAND_LENGTH + 1] = {L'\0'};
            size_t commandLength = 0;

            for( size_t index = 0; index != _arguments.size(); ++index )
            {
                WChar argument[MENGINE_MAX_PATH + 2];

                if( Helper::utf8ToUnicode( _arguments[index], argument, MENGINE_MAX_PATH + 2 ) == false )
                {
                    LOGGER_ERROR( "unable to convert process argument to Unicode" );

                    return false;
                }

                if( Detail::appendCommandLineArgument( argument, command, MENGINE_MAX_COMMAND_LENGTH - 1, &commandLength ) == false )
                {
                    LOGGER_ERROR( "process command is too long" );

                    return false;
                }
            }

            bool result = Helper::Win32CreateProcessW( executable, command, true, _exitCode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32CreateProcessA( const Char * _process, const WChar * _command, bool _wait, uint32_t * const _exitCode )
        {
            LOGGER_INFO( "platform", "create process '%s' command: %ls"
                , _process
                , _command
            );

            WPath unicode_process = {L'\0'};
            Helper::utf8ToUnicode( _process, unicode_process, MENGINE_MAX_PATH );

            bool result = Win32CreateProcessW( unicode_process, _command, _wait, _exitCode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32CreateProcessW( const WChar * _process, const WChar * _command, bool _wait, uint32_t * const _exitCode )
        {
            WChar unicode_command[MENGINE_MAX_COMMAND_LENGTH + 1] = {L'\0'};
            StdString::wcscpy_safe( unicode_command, L" ", MENGINE_MAX_COMMAND_LENGTH );
            StdString::wcscat_safe( unicode_command, _command, MENGINE_MAX_COMMAND_LENGTH );

            if( _wait == true )
            {
                SECURITY_ATTRIBUTES sa;
                ::ZeroMemory( &sa, sizeof( sa ) );

                sa.nLength = sizeof( sa );
                sa.lpSecurityDescriptor = NULL;
                sa.bInheritHandle = TRUE;

                HANDLE hReadPipe = NULL;
                HANDLE hWritePipe = NULL;

                if( ::CreatePipe( &hReadPipe, &hWritePipe, &sa, 0 ) == FALSE )
                {
                    LOGGER_ERROR( "CreatePipe get error %ls"
                        , Helper::Win32GetLastErrorMessageW()
                    );

                    return false;
                }

                ::SetHandleInformation( hReadPipe, HANDLE_FLAG_INHERIT, 0 );

                STARTUPINFO startupInfo;
                ::ZeroMemory( &startupInfo, sizeof( STARTUPINFO ) );

                startupInfo.cb = sizeof( STARTUPINFO );
                startupInfo.dwFlags = STARTF_USESTDHANDLES;
                startupInfo.hStdOutput = hWritePipe;
                startupInfo.hStdError = hWritePipe;
                startupInfo.hStdInput = NULL;

                PROCESS_INFORMATION processInfo;
                ::ZeroMemory( &processInfo, sizeof( PROCESS_INFORMATION ) );

                if( ::CreateProcessW( _process, unicode_command
                    , NULL
                    , NULL
                    , TRUE
                    , NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL
                    , NULL
                    , &startupInfo
                    , &processInfo ) == FALSE )
                {
                    LOGGER_ERROR( "CreateProcess '%ls' return %ls"
                        , _process
                        , Helper::Win32GetLastErrorMessageW()
                    );

                    ::CloseHandle( hWritePipe );
                    ::CloseHandle( hReadPipe );

                    return false;
                }

                ::CloseHandle( hWritePipe );

                Char outputBuffer[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};
                DWORD outputOffset = 0;

                for( ;; )
                {
                    DWORD nNumberOfBytesToRead = MENGINE_MIN( MENGINE_LOGGER_MAX_MESSAGE - outputOffset, (DWORD)MENGINE_LOGGER_MAX_MESSAGE );

                    if( nNumberOfBytesToRead == 0 )
                    {
                        break;
                    }

                    DWORD dwBytesRead = 0;
                    BOOL readResult = ::ReadFile( hReadPipe, outputBuffer + outputOffset, nNumberOfBytesToRead, &dwBytesRead, NULL );

                    if( readResult == FALSE || dwBytesRead == 0 )
                    {
                        break;
                    }

                    outputOffset += dwBytesRead;
                }

                ::CloseHandle( hReadPipe );

                outputBuffer[outputOffset] = '\0';

                ::WaitForSingleObject( processInfo.hProcess, INFINITE );

                DWORD exitCode = 0;
                BOOL result = ::GetExitCodeProcess( processInfo.hProcess, &exitCode );

                ::CloseHandle( processInfo.hProcess );
                ::CloseHandle( processInfo.hThread );

                if( result == FALSE )
                {
                    LOGGER_ERROR( "CreateProcess execute invalid get exit code\nprocess '%ls'\ncommand: %ls"
                        , _process
                        , _command
                    );

                    return false;
                }

                LOGGER_INFO( "platform", "process result '%ls' command '%ls' [%lu]"
                    , _process
                    , _command
                    , exitCode
                );

                if( outputOffset != 0 )
                {
                    if( exitCode == 0 )
                    {
                        LOGGER_MESSAGE( "%s", outputBuffer );
                    }
                    else
                    {
                        LOGGER_ERROR( "%s", outputBuffer );
                    }
                }

                if( _exitCode != nullptr )
                {
                    *_exitCode = (uint32_t)exitCode;
                }
            }
            else
            {
                STARTUPINFO startupInfo = {0};
                startupInfo.cb = sizeof( STARTUPINFO );

                PROCESS_INFORMATION processInfo;
                ::ZeroMemory( &processInfo, sizeof( PROCESS_INFORMATION ) );

                if( ::CreateProcessW( _process, unicode_command
                    , NULL
                    , NULL
                    , FALSE
                    , CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL
                    , NULL
                    , &startupInfo
                    , &processInfo ) == FALSE )
                {
                    LOGGER_ERROR( "invalid CreateProcess '%ls' command '%ls' error: %ls"
                        , _process
                        , _command
                        , Helper::Win32GetLastErrorMessageW()
                    );

                    return false;
                }

                ::CloseHandle( processInfo.hProcess );
                ::CloseHandle( processInfo.hThread );
            }

            return true;
        }
    }
}