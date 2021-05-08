#include "Win32FileHelper.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Win32Helper.h"

#include "Config/StdString.h"

#include "stdex/memorycopy.h"

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
                LOGGER_ERROR( "invalid create file '%ls' %s"
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
                LOGGER_ERROR( "file invalid find ??? (%ls) %s"
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
        size_t Win32ConcatenateFilePathA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity )
        {
            FilePath::size_type relationSize = _relationPath.size();
            FilePath::size_type folderSize = _folderPath.size();
            FilePath::size_type fileSize = _filePath.size();

            FilePath::size_type filePathSize = relationSize + folderSize + fileSize;

            if( filePathSize >= MENGINE_MAX_PATH )
            {
                LOGGER_ERROR( "invalid full path max size:\nrelation: %s\nfolder: %s\nfile: %s\ntotal size %u [max size: %zu]"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , filePathSize
                    , _capacity
                );

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
        size_t Win32ConcatenateFilePathW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _concatenatePath, size_t _capacity )
        {
            Char utf8_filePath[MENGINE_MAX_PATH] = {'\0'};
            size_t utf8_filePathLen = Helper::Win32ConcatenateFilePathA( _relationPath, _folderPath, _filePath, utf8_filePath, MENGINE_MAX_PATH );

            if( utf8_filePathLen == MENGINE_PATH_INVALID_LENGTH )
            {
                return MENGINE_PATH_INVALID_LENGTH;
            }

            if( UNICODE_SYSTEM()
                ->utf8ToUnicode( utf8_filePath, utf8_filePathLen, _concatenatePath, _capacity, nullptr ) == false )
            {
                LOGGER_ERROR( "invalid utf8 '%s' to unicode"
                    , utf8_filePath
                );

                return MENGINE_PATH_INVALID_LENGTH;
            }

            return utf8_filePathLen;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathTempA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity )
        {
            FilePath::size_type relationSize = _relationPath.size();
            FilePath::size_type folderSize = _folderPath.size();
            FilePath::size_type fileSize = _filePath.size();

            FilePath::size_type filePathSize = relationSize + folderSize + fileSize + 5;

            if( filePathSize >= MENGINE_MAX_PATH )
            {
                LOGGER_ERROR( "invalid full path max size:\nrelation: %s\nfolder: %s\nfile: %s\ntotal size %u [max size: %zu]"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , filePathSize
                    , _capacity
                );

                return MENGINE_PATH_INVALID_LENGTH;
            }

            stdex::memorycopy_safe( _concatenatePath, 0, _capacity, _relationPath.c_str(), relationSize );
            stdex::memorycopy_safe( _concatenatePath, relationSize, _capacity, _folderPath.c_str(), folderSize );
            stdex::memorycopy_safe( _concatenatePath, relationSize + folderSize, _capacity, _filePath.c_str(), fileSize );
            stdex::memorycopy_safe( _concatenatePath, relationSize + folderSize + fileSize, _capacity, ".~tmp", 5 );

            _concatenatePath[filePathSize] = '\0';

            Helper::pathCorrectBackslashA( _concatenatePath );

            return filePathSize;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathTempW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _concatenatePath, size_t _capacity )
        {
            Char utf8_filePath[MENGINE_MAX_PATH] = {'\0'};
            size_t utf8_filePathLen = Helper::Win32ConcatenateFilePathTempA( _relationPath, _folderPath, _filePath, utf8_filePath, MENGINE_MAX_PATH );

            if( utf8_filePathLen == MENGINE_PATH_INVALID_LENGTH )
            {
                return MENGINE_PATH_INVALID_LENGTH;
            }

            if( UNICODE_SYSTEM()
                ->utf8ToUnicode( utf8_filePath, utf8_filePathLen, _concatenatePath, _capacity, nullptr ) == false )
            {
                LOGGER_ERROR( "invalid utf8 '%s' to unicode"
                    , utf8_filePath
                );

                return MENGINE_PATH_INVALID_LENGTH;
            }

            return utf8_filePathLen;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}