#include "Win32FileHelper.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        HANDLE Win32CreateFile( const WChar * _filePath, DWORD _desiredAccess, DWORD _sharedMode, DWORD _creationDisposition )
        {
            WChar pathCorrect[MENGINE_MAX_PATH];
            Helper::pathCorrectBackslashToW( pathCorrect, _filePath );

            HANDLE handle = ::CreateFile( pathCorrect, _desiredAccess, _sharedMode, NULL,
                _creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );

            if( handle == INVALID_HANDLE_VALUE )
            {
                DWORD err_code = GetLastError();

                LOGGER_ERROR( "invalid create file '%ls' err %d"
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

            ::FindClose( hFind );

            const WChar * filename = PathFindFileName( _path );

            if( wcscmp( filename, wfd.cFileName ) != 0 )
            {
                LOGGER_ERROR( "File invalid name lowercase|upcase:\npath - '%ls'\nneed file name - '%ls'\ncurrent file name - '%ls'\n\n"
                    , _path
                    , filename
                    , wfd.cFileName
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _concatenatePath, size_t _capacity )
        {
            size_t relationSize = _relationPath.size();
            size_t folderSize = _folderPath.size();
            size_t fileSize = _filePath.size();

            size_t filePathSize = relationSize + folderSize + fileSize;

            if( filePathSize >= MENGINE_MAX_PATH )
            {
                return MENGINE_PATH_INVALID_LENGTH;
            }

            stdex::memorycopy_safe( _concatenatePath, 0, _capacity, _relationPath.c_str(), relationSize );
            stdex::memorycopy_safe( _concatenatePath, relationSize, _capacity, _folderPath.c_str(), folderSize );
            stdex::memorycopy_safe( _concatenatePath, relationSize + folderSize, _capacity, _filePath.c_str(), fileSize );

            _concatenatePath[filePathSize] = '\0';

            Helper::pathCorrectBackslashA( _concatenatePath );

            return filePathSize;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * _concatenatePath, size_t _capacity )
        {
            Char utf8_filePath[MENGINE_MAX_PATH];
            size_t utf8_filePathLen = Win32ConcatenateFilePathA( _relationPath, _folderPath, _filePath, utf8_filePath, MENGINE_MAX_PATH );

            if( utf8_filePathLen == MENGINE_PATH_INVALID_LENGTH )
            {
                return MENGINE_PATH_INVALID_LENGTH;
            }

            if( UNICODE_SYSTEM()
                ->utf8ToUnicode( utf8_filePath, utf8_filePathLen, _concatenatePath, _capacity, nullptr ) == false )
            {
                return MENGINE_PATH_INVALID_LENGTH;
            }

            return utf8_filePathLen;
        }
    }
}