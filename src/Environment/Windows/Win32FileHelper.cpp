#include "Win32FileHelper.h"

#include "Win32Helper.h"

#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        time_t Win32FileTimeToUnixTime( const FILETIME * _filetime )
        {
            uint32_t a0;
            uint32_t a1;
            uint32_t a2;

            uint32_t carry;
            int negative;

            a2 = _filetime->dwHighDateTime;
            a1 = ((uint32_t)_filetime->dwLowDateTime) >> 16;
            a0 = ((uint32_t)_filetime->dwLowDateTime) & 0xffff;

            if( a0 >= 32768 )
                a0 -= 32768, carry = 0;
            else
                a0 += (1 << 16) - 32768, carry = 1;

            if( a1 >= 54590 + carry )
                a1 -= 54590 + carry, carry = 0;
            else
                a1 += (1 << 16) - 54590 - carry, carry = 1;

            a2 -= 27111902 + carry;

            negative = (a2 >= ((uint32_t)1) << 31);
            if( negative )
            {
                a0 = 0xffff - a0;
                a1 = 0xffff - a1;
                a2 = ~a2;
            }

            a1 += (a2 % 10000) << 16;
            a2 /= 10000;
            a0 += (a1 % 10000) << 16;
            a1 /= 10000;
            a0 /= 10000;

            a1 += (a2 % 1000) << 16;
            a2 /= 1000;
            a0 += (a1 % 1000) << 16;
            a1 /= 1000;
            a0 /= 1000;

            if( negative )
            {
                /* Set a to -a - 1 (a is a2/a1/a0) */
                a0 = 0xffff - a0;
                a1 = 0xffff - a1;
                a2 = ~a2;
            }

            return ((((time_t)a2) << 16) << 16) + ((time_t)a1 << 16) + a0;
        }
        //////////////////////////////////////////////////////////////////////////
        HANDLE Win32CreateFile( const WChar * _filePath, DWORD _desiredAccess, DWORD _sharedMode, DWORD _creationDisposition )
        {
            WChar pathCorrect[MENGINE_MAX_PATH + 1] = {L'\0'};
            Helper::pathCorrectBackslashToW( pathCorrect, _filePath );

            HANDLE handle = ::CreateFileW(
                pathCorrect, //lpFileName
                _desiredAccess, //dwDesiredAccess
                _sharedMode, //dwShareMode
                NULL, //lpSecurityAttributes
                _creationDisposition, //dwCreationDisposition
                FILE_ATTRIBUTE_NORMAL, //dwFlagsAndAttributes
                NULL //hTemplateFile
            );

            if( handle == INVALID_HANDLE_VALUE )
            {
                LOGGER_ERROR( "invalid create file '%ls' %ls"
                    , pathCorrect
                    , Helper::Win32GetLastErrorMessageW()
                );

                return INVALID_HANDLE_VALUE;
            }

#if defined(MENGINE_DEBUG)
            if( Helper::Win32ValidateFile( pathCorrect ) == false )
            {
                ::CloseHandle( handle );

                return INVALID_HANDLE_VALUE;
            }
#endif

            return handle;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32ValidateFile( const WChar * _filePath )
        {
            WChar pathCorrect[MENGINE_MAX_PATH + 1] = {L'\0'};
            Helper::pathCorrectBackslashToW( pathCorrect, _filePath );

            WIN32_FIND_DATA wfd;
            HANDLE hFind = ::FindFirstFile( pathCorrect, &wfd );

            if( hFind == INVALID_HANDLE_VALUE )
            {
                LOGGER_ERROR( "file invalid find ??? (%ls) get error %ls"
                    , pathCorrect
                    , Helper::Win32GetLastErrorMessageW()
                );

                return false;
            }

            ::FindClose( hFind );

            const WChar * filename = ::PathFindFileNameW( pathCorrect );

            if( StdString::wcscmp( filename, wfd.cFileName ) != 0 )
            {
                LOGGER_ERROR( "file invalid name lowercase|upcase:\npath - '%ls'\nneed file name - '%ls'\ncurrent file name - '%ls'\n\n"
                    , pathCorrect
                    , filename
                    , wfd.cFileName
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32ExistFile( const WChar * _filePath )
        {
            WChar pathCorrect[MENGINE_MAX_PATH + 1] = {L'\0'};
            Helper::pathCorrectBackslashToW( pathCorrect, _filePath );

            size_t len = StdString::wcslen( pathCorrect );

            if( len == 0 )
            {
                return false;
            }

            if( pathCorrect[len - 1] == L':' )
            {
                return true;
            }

            DWORD attributes = ::GetFileAttributesW( pathCorrect );

            if( attributes == INVALID_FILE_ATTRIBUTES )
            {
                return false;
            }

            if( (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32ExistDirectory( const WChar * _basePath, const WChar * _directory )
        {
            WChar pathDirectory[MENGINE_MAX_PATH + 1] = {L'\0'};
            Helper::pathCorrectBackslashToW( pathDirectory, _directory );

            Helper::pathRemoveFileSpecW( pathDirectory );

            size_t len = StdString::wcslen( pathDirectory );

            if( len == 0 )
            {
                return true;
            }

            WChar pathFull[MENGINE_MAX_PATH + 1] = {'\0'};
            StdString::wcsncpy( pathFull, _basePath, MENGINE_MAX_PATH );
            StdString::wcsncat( pathFull, pathDirectory, MENGINE_MAX_PATH );

            if( pathFull[len - 1] == L':' )
            {
                return true;
            }

            DWORD attributes = ::GetFileAttributes( pathFull );

            if( attributes == INVALID_FILE_ATTRIBUTES )
            {
                return false;
            }

            if( (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32CreateDirectory( const WChar * _basePath, const WChar * _directory )
        {
            size_t unicode_directorySize = StdString::wcslen( _directory );

            if( unicode_directorySize == 0 )
            {
                return true;
            }

            WChar pathDirectory[MENGINE_MAX_PATH + 1] = {L'\0'};
            Helper::pathCorrectBackslashToW( pathDirectory, _directory );

            Helper::pathRemoveFileSpecW( pathDirectory );

            if( StdString::wcslen( pathDirectory ) == 0 )
            {
                return true;
            }

            WChar pathTestDirectory[MENGINE_MAX_PATH + 1] = {L'\0'};
            StdString::wcsncpy( pathTestDirectory, _basePath, MENGINE_MAX_PATH );
            StdString::wcsncat( pathTestDirectory, pathDirectory, MENGINE_MAX_PATH );

            if( ::PathIsDirectoryW( pathTestDirectory ) == FILE_ATTRIBUTE_DIRECTORY )
            {
                return true;
            }

            Helper::pathRemoveBackslashW( pathDirectory );

            uint32_t paths_count = 0;
            WChar paths[MENGINE_MAX_PATH + 1][16];

            for( ;; )
            {
                StdString::wcsncpy( paths[paths_count++], pathDirectory, MENGINE_MAX_PATH );

                if( Helper::pathRemoveFileSpecW( pathDirectory ) == false )
                {
                    break;
                }

                Helper::pathRemoveBackslashW( pathDirectory );

                StdString::wcsncpy( pathTestDirectory, _basePath, MENGINE_MAX_PATH );
                StdString::wcsncat( pathTestDirectory, pathDirectory, MENGINE_MAX_PATH );

                if( ::PathIsDirectoryW( pathTestDirectory ) == FILE_ATTRIBUTE_DIRECTORY )
                {
                    break;
                }
            }

            for( uint32_t index = paths_count; index != 0; --index )
            {
                const WChar * path = paths[index - 1];

                WChar pathCreateDirectory[MENGINE_MAX_PATH + 1] = {L'\0'};
                StdString::wcsncpy( pathCreateDirectory, _basePath, MENGINE_MAX_PATH );
                StdString::wcsncat( pathCreateDirectory, path, MENGINE_MAX_PATH );

                BOOL successful = ::CreateDirectory( pathCreateDirectory, NULL );

                if( successful == FALSE )
                {
                    DWORD error = ::GetLastError();

                    switch( error )
                    {
                    case ERROR_ALREADY_EXISTS:
                        {
                            continue;
                        }break;
                    case ERROR_PATH_NOT_FOUND:
                        {
                            LOGGER_ERROR( "directory '%ls' not found"
                                , pathCreateDirectory
                            );

                            return false;
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "directory '%ls' unknown %ls"
                                , pathCreateDirectory
                                , Helper::Win32GetLastErrorMessageW()
                            );

                            return false;
                        }break;
                    }
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32ListDirectory( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaListDirectoryFilePath & _lambda, bool * const _stop )
        {
            {
                WChar sPath[MENGINE_MAX_PATH + 1] = {L'\0'};
                StdString::wcscpy( sPath, _dir );
                StdString::wcscat( sPath, _path );
                StdString::wcscat( sPath, _mask );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind != INVALID_HANDLE_VALUE )
                {
                    do
                    {
                        if( StdString::wcscmp( fdFile.cFileName, L"." ) == 0 ||
                            StdString::wcscmp( fdFile.cFileName, L".." ) == 0 )
                        {
                            continue;
                        }

                        if( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                        {
                            continue;
                        }

                        WChar sPath2[MENGINE_MAX_PATH + 1] = {L'\0'};
                        StdString::wcscpy( sPath2, sPath );
                        StdString::wcscat( sPath2, L"\0" );

                        Helper::pathCorrectForwardslashW( sPath2 );

                        ::PathRemoveFileSpecW( sPath2 );

                        WChar unicode_filepath[MENGINE_MAX_PATH + 1] = {L'\0'};
                        ::PathCombineW( unicode_filepath, sPath2, fdFile.cFileName );

                        WChar unicode_out[MENGINE_MAX_PATH + 1] = {L'\0'};
                        if( StdString::wcslen( _dir ) != 0 )
                        {
                            ::PathRelativePathToW( unicode_out,
                                _dir,
                                FILE_ATTRIBUTE_DIRECTORY,
                                unicode_filepath,
                                FILE_ATTRIBUTE_NORMAL );
                        }
                        else
                        {
                            StdString::wcscpy( unicode_out, unicode_filepath );
                        }


                        if( _lambda( unicode_out ) == false )
                        {
                            ::FindClose( hFind );

                            *_stop = true;

                            return true;
                        }

                    } while( ::FindNextFile( hFind, &fdFile ) != FALSE );
                }

                ::FindClose( hFind );
            }

            {
                WChar sPath[MENGINE_MAX_PATH + 1] = {L'\0'};
                StdString::wcscpy( sPath, _dir );
                StdString::wcscat( sPath, _path );
                StdString::wcscat( sPath, L"*.*" );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileExW( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind == INVALID_HANDLE_VALUE )
                {
                    return true;
                }

                do
                {
                    if( StdString::wcscmp( fdFile.cFileName, L"." ) == 0 ||
                        StdString::wcscmp( fdFile.cFileName, L".." ) == 0 )
                    {
                        continue;
                    }

                    if( (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
                    {
                        continue;
                    }

                    WChar currentPath[MENGINE_MAX_PATH + 1] = {L'\0'};
                    StdString::wcscpy( currentPath, sPath );
                    StdString::wcscat( currentPath, L"\0" );

                    ::PathRemoveFileSpecW( currentPath );

                    WChar nextPath[MENGINE_MAX_PATH + 1] = {L'\0'};
                    ::PathCombineW( nextPath, currentPath, fdFile.cFileName );

                    ::PathAddBackslashW( nextPath );

                    bool stop;
                    if( Helper::Win32ListDirectory( _dir, _mask, nextPath, _lambda, &stop ) == false )
                    {
                        ::FindClose( hFind );

                        return false;
                    }

                    if( stop == true )
                    {
                        ::FindClose( hFind );

                        *_stop = true;

                        return true;
                    }

                } while( ::FindNextFile( hFind, &fdFile ) != FALSE );

                ::FindClose( hFind );
            }

            *_stop = false;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}