#include "PathHelper.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool concatenateFilePath( InitializerList<FilePath> && _paths, Char * const _concatenatePath )
        {
            if( Helper::memoryCopySafe( _concatenatePath, 0, MENGINE_MAX_PATH, "", 0, 0 ) == false )
            {
                return false;
            }

            size_t offset = 0;

            for( const FilePath & path : _paths )
            {
                const Char * path_str = path.c_str();
                FilePath::size_type path_size = path.size();

                if( Helper::memoryCopySafe( _concatenatePath, offset, MENGINE_MAX_PATH, path_str, 0, path_size ) == false )
                {
                    return false;
                }

                offset += path_size;
            }

            _concatenatePath[offset] = '\0';

            Helper::pathCorrectBackslashA( _concatenatePath );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath concatenateFilePath( InitializerList<FilePath> && _paths )
        {
            Char concatenatePath[MENGINE_MAX_PATH];
            if( Helper::concatenateFilePath( std::forward<InitializerList<FilePath>>( _paths ), concatenatePath ) == false )
            {
                return FilePath::none();
            }

            FilePath fp = Helper::stringizeFilePath( concatenatePath );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath getFolderPath( const FilePath & _fullpath )
        {
            const Char * str_fullpath = _fullpath.c_str();

            const Char * folder_delimiter_1 = StdString::strrchr( str_fullpath, MENGINE_PATH_DELIM_FORWARDSLASH );
            const Char * folder_delimiter_2 = StdString::strrchr( str_fullpath, MENGINE_PATH_DELIM_BACKSLASH );

            const Char * folder_delimiter = (folder_delimiter_1 > folder_delimiter_2) ? folder_delimiter_1 : folder_delimiter_2;

            if( folder_delimiter == nullptr )
            {
                return FilePath( ConstString::none() );
            }

            FilePath::size_type size_fullpath = (FilePath::size_type)(folder_delimiter - str_fullpath + 1);

            FilePath c_folder = Helper::stringizeFilePathSize( str_fullpath, size_fullpath );

            return c_folder;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath replaceFileSpec( const FilePath & _fullpath, const Char * _fileSpec )
        {
            const Char * str_fullpath = _fullpath.c_str();

            const Char * folder_delimiter_1 = StdString::strrchr( str_fullpath, MENGINE_PATH_DELIM_FORWARDSLASH );
            const Char * folder_delimiter_2 = StdString::strrchr( str_fullpath, MENGINE_PATH_DELIM_BACKSLASH );

            const Char * folder_delimiter = (folder_delimiter_1 > folder_delimiter_2) ? folder_delimiter_1 : folder_delimiter_2;

            if( folder_delimiter == nullptr )
            {
                return Helper::stringizeFilePath( _fileSpec );
            }

            FilePath::size_type size_fullpath = (FilePath::size_type)(folder_delimiter - str_fullpath + 1);

            FilePath c_folder = Helper::stringizeFilePathFormat( "%.*s%s"
                , (int32_t)size_fullpath
                , str_fullpath
                , _fileSpec
            );

            return c_folder;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath unicodeToFilePath( const WString & _unicode )
        {
            String utf8;
            if( Helper::unicodeToUtf8Size( _unicode.c_str(), _unicode.size(), &utf8 ) == false )
            {
                return FilePath::none();
            }

            FilePath fp = Helper::stringizeFilePath( utf8 );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isCorrectFolderPathW( const WChar * _path )
        {
            size_t path_size = StdString::wcslen( _path );

            if( path_size == 0 )
            {
                return true;
            }

            WChar delim = _path[path_size - 1];

            if( delim == MENGINE_PATH_WDELIM_FORWARDSLASH ||
                delim == MENGINE_PATH_WDELIM_BACKSLASH )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isCorrectFolderPathA( const Char * _path )
        {
            size_t path_size = StdString::strlen( _path );

            if( path_size == 0 )
            {
                return true;
            }

            Char delim = _path[path_size - 1];

            if( delim == MENGINE_PATH_DELIM_FORWARDSLASH ||
                delim == MENGINE_PATH_DELIM_BACKSLASH )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCombineW( WChar * const _out, const WChar * _base, const WChar * _path, WChar _slash )
        {
            size_t base_lenght = StdString::wcslen( _base );

            if( base_lenght == 0 )
            {
                StdString::wcscpy( _out, _path );

                return;
            }

            StdString::wcscpy( _out, _base );

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::wcschrcat( _out, _slash );
            }

            StdString::wcscat( _out, _path );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCombineA( Char * const _out, const Char * _base, const Char * _path, Char _slash )
        {
            size_t base_lenght = StdString::strlen( _base );

            if( base_lenght == 0 )
            {
                StdString::strcpy( _out, _path );

                return;
            }

            StdString::strcpy( _out, _base );

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::strchrcat( _out, _slash );
            }

            StdString::strcat( _out, _path );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathAppendW( WChar * const _base, const WChar * _path, WChar _slash )
        {
            size_t base_lenght = StdString::wcslen( _base );

            if( base_lenght == 0 )
            {
                StdString::wcscpy( _base, _path );

                return;
            }

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::wcschrcat( _base, _slash );
            }

            StdString::wcscat( _base, _path );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathAppendA( Char * const _base, const Char * _path, Char _slash )
        {
            size_t base_lenght = StdString::strlen( _base );

            if( base_lenght == 0 )
            {
                StdString::strcpy( _base, _path );

                return;
            }

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::strchrcat( _base, _slash );
            }

            StdString::strcat( _base, _path );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectFolderPathW( WChar * const _folderPath, WChar _delim )
        {
            size_t len = StdString::wcslen( _folderPath );

            if( len == 0 )
            {
                return;
            }

            WChar * pch = _folderPath + len - 1;

            if( *pch == _delim )
            {
                return;
            }

            StdString::wcsnchrcat( _folderPath, _delim, MENGINE_MAX_PATH );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectFolderPathA( Char * const _folderPath, Char _delim )
        {
            size_t len = StdString::strlen( _folderPath );

            if( len == 0 )
            {
                return;
            }

            Char * pch = _folderPath + len - 1;

            if( *pch == _delim )
            {
                return;
            }

            StdString::strnchrcat( _folderPath, _delim, MENGINE_MAX_PATH );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashW( WChar * const _filePath )
        {
            WChar * pch = StdString::wcschr( _filePath, MENGINE_PATH_WDELIM_FORWARDSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_WDELIM_BACKSLASH;

                pch = StdString::wcschr( pch, MENGINE_PATH_WDELIM_FORWARDSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashA( Char * const _filePath )
        {
            Char * pch = StdString::strchr( _filePath, MENGINE_PATH_DELIM_FORWARDSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_DELIM_BACKSLASH;

                pch = StdString::strchr( pch, MENGINE_PATH_DELIM_FORWARDSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToW( WChar * const _out, const WChar * _filePath )
        {
            StdString::wcscpy( _out, _filePath );

            Helper::pathCorrectBackslashW( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToA( Char * const _out, const Char * _filePath )
        {
            StdString::strcpy( _out, _filePath );

            Helper::pathCorrectBackslashA( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashW( WChar * const _filePath )
        {
            WChar * pch = StdString::wcschr( _filePath, MENGINE_PATH_WDELIM_BACKSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_WDELIM_FORWARDSLASH;

                pch = StdString::wcschr( pch, MENGINE_PATH_WDELIM_BACKSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashA( Char * const _filePath )
        {
            Char * pch = StdString::strchr( _filePath, MENGINE_PATH_DELIM_BACKSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_DELIM_FORWARDSLASH;

                pch = StdString::strchr( pch, MENGINE_PATH_DELIM_BACKSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashToW( WChar * const _out, const WChar * _filePath )
        {
            StdString::wcscpy( _out, _filePath );

            Helper::pathCorrectForwardslashW( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashToA( Char * const _out, const Char * _filePath )
        {
            StdString::strcpy( _out, _filePath );

            Helper::pathCorrectForwardslashA( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveSlashW( WChar * const _filePath, WChar _delim )
        {
            size_t len = StdString::wcslen( _filePath );

            WChar * pch = _filePath + len - 1;

            if( *pch == _delim )
            {
                *pch = L'\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveSlashA( Char * const _filePath, Char _delim )
        {
            size_t len = StdString::strlen( _filePath );

            Char * pch = _filePath + len - 1;

            if( *pch == _delim )
            {
                *pch = '\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecW( WChar * const _filePath, WChar _delim )
        {
            size_t len = StdString::wcslen( _filePath );

            if( len == 0 )
            {
                return false;
            }

            WChar * pch = _filePath + len - 1;

            if( *pch == _delim )
            {
                return false;
            }

            pch--;

            while( *pch != _delim )
            {
                if( pch == _filePath )
                {
                    *pch = L'\0';

                    return false;
                }

                pch--;
            }

            pch++;

            *pch = L'\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecA( Char * const _filePath, Char _delim )
        {
            size_t len = StdString::strlen( _filePath );

            if( len == 0 )
            {
                return false;
            }

            Char * pch = _filePath + len - 1;

            if( *pch == _delim )
            {
                return false;
            }

            pch--;

            while( *pch != _delim )
            {
                if( pch == _filePath )
                {
                    *pch = '\0';

                    return false;
                }

                pch--;
            }

            pch++;

            *pch = '\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}