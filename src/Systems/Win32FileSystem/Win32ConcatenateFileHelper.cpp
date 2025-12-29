#include "Win32ConcatenateFileHelper.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryCopy.h"

#include "Config/StdString.h"
#include "Config/Path.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath )
        {
            FilePath::size_type relationSize = _relationPath.size();
            FilePath::size_type folderSize = _folderPath.size();
            FilePath::size_type fileSize = _filePath.size();

            FilePath::size_type filePathSize = relationSize + folderSize + fileSize;

            if( filePathSize >= MENGINE_MAX_PATH )
            {
                LOGGER_ERROR( "invalid full path max size:\nrelation: %s\nfolder: %s\nfile: %s\ntotal size %zu"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , filePathSize
                );

                return MENGINE_PATH_INVALID_LENGTH;
            }

            Helper::memoryCopySafe( _concatenatePath, 0, MENGINE_MAX_PATH, _relationPath.c_str(), 0, relationSize );
            Helper::memoryCopySafe( _concatenatePath, relationSize, MENGINE_MAX_PATH, _folderPath.c_str(), 0, folderSize );
            Helper::memoryCopySafe( _concatenatePath, relationSize + folderSize, MENGINE_MAX_PATH, _filePath.c_str(), 0, fileSize );

            _concatenatePath[filePathSize] = '\0';

            Helper::pathCorrectBackslashA( _concatenatePath );

            return filePathSize;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _concatenatePath )
        {
            Path utf8_filePath = {'\0'};
            size_t utf8_filePathLen = Helper::Win32ConcatenateFilePathA( _relationPath, _folderPath, _filePath, utf8_filePath );

            if( utf8_filePathLen == MENGINE_PATH_INVALID_LENGTH )
            {
                return MENGINE_PATH_INVALID_LENGTH;
            }

            if( UNICODE_SYSTEM()
                ->utf8ToUnicode( utf8_filePath, utf8_filePathLen, _concatenatePath, MENGINE_MAX_PATH, nullptr ) == false )
            {
                LOGGER_ERROR( "invalid utf8 '%s' to unicode"
                    , utf8_filePath
                );

                return MENGINE_PATH_INVALID_LENGTH;
            }

            return utf8_filePathLen;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathTempA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath )
        {
            FilePath::size_type relationSize = _relationPath.size();
            FilePath::size_type folderSize = _folderPath.size();
            FilePath::size_type fileSize = _filePath.size();

            FilePath::size_type filePathSize = relationSize + folderSize + fileSize + 5;

            if( filePathSize >= MENGINE_MAX_PATH )
            {
                LOGGER_ERROR( "invalid full path max size:\nrelation: %s\nfolder: %s\nfile: %s\ntotal size %zu"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , filePathSize
                );

                return MENGINE_PATH_INVALID_LENGTH;
            }

            Helper::memoryCopySafe( _concatenatePath, 0, MENGINE_MAX_PATH, _relationPath.c_str(), 0, relationSize );
            Helper::memoryCopySafe( _concatenatePath, relationSize, MENGINE_MAX_PATH, _folderPath.c_str(), 0, folderSize );
            Helper::memoryCopySafe( _concatenatePath, relationSize + folderSize, MENGINE_MAX_PATH, _filePath.c_str(), 0, fileSize );
            Helper::memoryCopySafe( _concatenatePath, relationSize + folderSize + fileSize, MENGINE_MAX_PATH, ".~tmp", 0, 5 );

            _concatenatePath[filePathSize] = '\0';

            Helper::pathCorrectBackslashA( _concatenatePath );

            return filePathSize;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t Win32ConcatenateFilePathTempW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _concatenatePath )
        {
            Path utf8_filePath = {'\0'};
            size_t utf8_filePathLen = Helper::Win32ConcatenateFilePathTempA( _relationPath, _folderPath, _filePath, utf8_filePath );

            if( utf8_filePathLen == MENGINE_PATH_INVALID_LENGTH )
            {
                return MENGINE_PATH_INVALID_LENGTH;
            }

            if( UNICODE_SYSTEM()
                ->utf8ToUnicode( utf8_filePath, utf8_filePathLen, _concatenatePath, MENGINE_MAX_PATH, nullptr ) == false )
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