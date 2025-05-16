#include "Win32CreateProcess.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"

#include "Config/StdString.h"
#include "Config/Path.h"

namespace Mengine
{
    namespace Helper
    {
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
            StdString::wcscpy( unicode_command, L" " );
            StdString::wcscat( unicode_command, _command );

            if( _wait == true )
            {
                SECURITY_ATTRIBUTES sa;
                ::ZeroMemory( &sa, sizeof( sa ) );

                sa.nLength = sizeof( sa );
                sa.lpSecurityDescriptor = NULL;
                sa.bInheritHandle = TRUE;

                WPath tempPathBuffer = {L'\0'};
                ::GetTempPathW( MENGINE_MAX_PATH, tempPathBuffer );

                WPath tempFileNameBuffer = {L'\0'};
                ::GetTempFileNameW( tempPathBuffer
                    , L"Process"
                    , 0
                    , tempFileNameBuffer );

                HANDLE hWriteTempFile = ::CreateFileW( tempFileNameBuffer
                    , FILE_APPEND_DATA
                    , FILE_SHARE_WRITE | FILE_SHARE_READ
                    , &sa
                    , OPEN_ALWAYS
                    , FILE_ATTRIBUTE_TEMPORARY
                    , NULL );

                if( hWriteTempFile == INVALID_HANDLE_VALUE )
                {
                    LOGGER_ERROR( "CreateFile '%ls' get error %ls"
                        , tempFileNameBuffer
                        , Helper::Win32GetLastErrorMessageW()
                    );

                    return false;
                }

                STARTUPINFO startupInfo;
                ::ZeroMemory( &startupInfo, sizeof( STARTUPINFO ) );

                startupInfo.cb = sizeof( STARTUPINFO );
                startupInfo.dwFlags = STARTF_USESTDHANDLES;
                startupInfo.hStdOutput = hWriteTempFile;
                startupInfo.hStdError = hWriteTempFile;
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

                    return false;
                }

                ::WaitForSingleObject( processInfo.hProcess, INFINITE );

                DWORD exitCode = 0;
                BOOL result = ::GetExitCodeProcess( processInfo.hProcess, &exitCode );

                ::CloseHandle( processInfo.hProcess );
                ::CloseHandle( processInfo.hThread );
                ::CloseHandle( hWriteTempFile );

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

                HANDLE hReadTempFile = ::CreateFileW( tempFileNameBuffer
                    , FILE_GENERIC_READ
                    , FILE_SHARE_WRITE | FILE_SHARE_READ
                    , &sa
                    , OPEN_ALWAYS
                    , FILE_ATTRIBUTE_TEMPORARY
                    , NULL );

                if( hReadTempFile == INVALID_HANDLE_VALUE )
                {
                    LOGGER_ERROR( "process '%ls' command '%ls' CreateFile '%ls' get error %ls"
                        , _process
                        , _command
                        , tempFileNameBuffer
                        , Helper::Win32GetLastErrorMessageW()
                    );

                    return false;
                }

                DWORD tempFileSizeHigh;
                DWORD tempFileSize = ::GetFileSize( hReadTempFile, &tempFileSizeHigh );

                if( tempFileSize != 0 )
                {
                    Char tempFileBuffer[4096 + 1] = {'\0'};

                    DWORD dwBytesRead;
                    DWORD nNumberOfBytesToRead = MENGINE_MIN( tempFileSize, 4095 );
                    BOOL successful = ::ReadFile( hReadTempFile, tempFileBuffer, nNumberOfBytesToRead, &dwBytesRead, NULL );

                    if( successful == TRUE )
                    {
                        if( exitCode == 0 )
                        {
                            LOGGER_INFO( "platform", "%s"
                                , tempFileBuffer
                            );
                        }
                        else
                        {
                            LOGGER_ERROR( "%s"
                                , tempFileBuffer
                            );
                        }
                    }
                    else
                    {
                        LOGGER_ERROR( "invalid read file '%ls'"
                            , tempFileNameBuffer
                        );
                    }
                }

                ::CloseHandle( hReadTempFile );

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