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

            HANDLE handle = ::CreateFile(
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
                    , Helper::Win32GetLastErrorMessage()
                );

                return INVALID_HANDLE_VALUE;
            }

#ifdef MENGINE_DEBUG
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
                    , Helper::Win32GetLastErrorMessage()
                );

                return false;
            }

            ::FindClose( hFind );

            const WChar * filename = ::PathFindFileName( _path );

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
    }
}