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
        HANDLE Win32CreateFile( const WChar * _filePath, DWORD _desiredAccess, DWORD _sharedMode, DWORD _creationDisposition )
        {
            WChar pathCorrect[MENGINE_MAX_PATH] = {L'\0'};
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
        bool Win32ValidateFile( const WChar * _path )
        {
            WIN32_FIND_DATA wfd;
            HANDLE hFind = ::FindFirstFile( _path, &wfd );

            if( hFind == INVALID_HANDLE_VALUE )
            {
                LOGGER_ERROR( "file invalid find ??? (%ls) get error %ls"
                    , _path
                    , Helper::Win32GetLastErrorMessageW()
                );

                return false;
            }

            ::FindClose( hFind );

            const WChar * filename = ::PathFindFileNameW( _path );

            if( MENGINE_WCSCMP( filename, wfd.cFileName ) != 0 )
            {
                LOGGER_ERROR( "file invalid name lowercase|upcase:\npath - '%ls'\nneed file name - '%ls'\ncurrent file name - '%ls'\n\n"
                    , _path
                    , filename
                    , wfd.cFileName
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32ListDirectory( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaListDirectoryFilePath & _lambda, bool * const _stop )
        {
            {
                WChar sPath[MENGINE_MAX_PATH] = {L'\0'};
                MENGINE_WCSCPY( sPath, _dir );
                MENGINE_WCSCAT( sPath, _path );
                MENGINE_WCSCAT( sPath, _mask );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind != INVALID_HANDLE_VALUE )
                {
                    do
                    {
                        if( MENGINE_WCSCMP( fdFile.cFileName, L"." ) == 0 ||
                            MENGINE_WCSCMP( fdFile.cFileName, L".." ) == 0 )
                        {
                            continue;
                        }

                        if( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                        {
                            continue;
                        }

                        WChar sPath2[MENGINE_MAX_PATH] = {L'\0'};
                        MENGINE_WCSCPY( sPath2, sPath );
                        MENGINE_WCSCAT( sPath2, L"\0" );

                        Helper::pathCorrectForwardslashW( sPath2 );

                        ::PathRemoveFileSpecW( sPath2 );

                        WChar unicode_filepath[MENGINE_MAX_PATH] = {L'\0'};
                        ::PathCombineW( unicode_filepath, sPath2, fdFile.cFileName );

                        WChar unicode_out[MENGINE_MAX_PATH] = {L'\0'};
                        if( MENGINE_WCSLEN( _dir ) != 0 )
                        {
                            ::PathRelativePathToW( unicode_out,
                                _dir,
                                FILE_ATTRIBUTE_DIRECTORY,
                                unicode_filepath,
                                FILE_ATTRIBUTE_NORMAL );
                        }
                        else
                        {
                            MENGINE_WCSCPY( unicode_out, unicode_filepath );
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
                WChar sPath[MENGINE_MAX_PATH] = {L'\0'};
                MENGINE_WCSCPY( sPath, _dir );
                MENGINE_WCSCAT( sPath, _path );
                MENGINE_WCSCAT( sPath, L"*.*" );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileExW( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind == INVALID_HANDLE_VALUE )
                {
                    return true;
                }

                do
                {
                    if( MENGINE_WCSCMP( fdFile.cFileName, L"." ) == 0 ||
                        MENGINE_WCSCMP( fdFile.cFileName, L".." ) == 0 )
                    {
                        continue;
                    }

                    if( (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
                    {
                        continue;
                    }

                    WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
                    MENGINE_WCSCPY( currentPath, sPath );
                    MENGINE_WCSCAT( currentPath, L"\0" );

                    ::PathRemoveFileSpecW( currentPath );

                    WChar nextPath[MENGINE_MAX_PATH] = {L'\0'};
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