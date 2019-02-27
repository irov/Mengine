#include "Win32FileHelper.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        HANDLE Win32CreateFile( const WChar * _filePath, DWORD _desiredAccess, DWORD _sharedMode, DWORD _creationDisposition )
        {
            WChar pathCorrect[MENGINE_MAX_PATH];
            Helper::pathCorrectBackslashTo( pathCorrect, _filePath );

            HANDLE handle = ::CreateFile( pathCorrect, _desiredAccess, _sharedMode, NULL,
                _creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );

            if( handle == INVALID_HANDLE_VALUE )
            {
                DWORD err_code = GetLastError();

                LOGGER_ERROR( "Win32CreateFile invalid create file %ls err %d"
                    , pathCorrect
                    , err_code
                );

                return INVALID_HANDLE_VALUE;
            }

#ifdef MENGINE_DEBUG
            if( Win32ValidateFile( pathCorrect ) == false )
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
                LOGGER_ERROR( "File invalid find ??? (%ls)\n"
                    , _path
                );

                return false;
            }

            const WChar * filename = PathFindFileName( _path );

            if( wcscmp( filename, wfd.cFileName ) != 0 )
            {
                LOGGER_ERROR( "File invalid name lowercase|upcase:\npath - '%ls'\nneed file name - '%ls'\ncurrent file name - '%ls'\n\n"
                    , _path
                    , filename
                    , wfd.cFileName
                );

                ::FindClose( hFind );

                return false;
            }

            ::FindClose( hFind );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32ConcatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * _concatenatePath, size_t _capacity )
        {
            size_t relationSize = _relationPath.size();
            size_t folderSize = _folderPath.size();
            size_t fileSize = _filePath.size();

            size_t filePathSize = relationSize + folderSize + fileSize;

            if( filePathSize >= MENGINE_MAX_PATH )
            {
                return false;
            }

            Char filePath[MENGINE_MAX_PATH];
            stdex::memorycopy( filePath, 0, _relationPath.c_str(), relationSize );
            stdex::memorycopy( filePath, relationSize, _folderPath.c_str(), folderSize );
            stdex::memorycopy( filePath, relationSize + folderSize, _filePath.c_str(), fileSize );

            filePath[filePathSize] = '\0';
            filePathSize += 1; //Null

            WChar filePathW[MENGINE_MAX_PATH];
            if( UNICODE_SYSTEM()
                ->utf8ToUnicode( filePath, filePathSize, filePathW, _capacity, nullptr ) == false )
            {
                return false;
            }

            Helper::pathCorrectBackslashTo( _concatenatePath, filePathW );

            return true;
        }
    }
}