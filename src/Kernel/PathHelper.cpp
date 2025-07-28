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

            const Char * folder_delimiter_1 = StdString::strrchr( str_fullpath, MENGINE_PATH_BACKSLASH );
            const Char * folder_delimiter_2 = StdString::strrchr( str_fullpath, MENGINE_PATH_FORWARDSLASH );

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

            const Char * folder_delimiter_1 = StdString::strrchr( str_fullpath, MENGINE_PATH_BACKSLASH );
            const Char * folder_delimiter_2 = StdString::strrchr( str_fullpath, MENGINE_PATH_FORWARDSLASH );

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

            if( delim == MENGINE_PATH_WBACKSLASH ||
                delim == MENGINE_PATH_WFORWARDSLASH )
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

            if( delim == MENGINE_PATH_BACKSLASH ||
                delim == MENGINE_PATH_FORWARDSLASH )
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
                StdString::wcscpy_safe( _out, _path, MENGINE_MAX_PATH );

                return;
            }

            StdString::wcscpy_safe( _out, _base, MENGINE_MAX_PATH );

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::wcschrcat_safe( _out, _slash, MENGINE_MAX_PATH );
            }

            StdString::wcscat_safe( _out, _path, MENGINE_MAX_PATH );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCombineA( Char * const _out, const Char * _base, const Char * _path, Char _slash )
        {
            size_t base_lenght = StdString::strlen( _base );

            if( base_lenght == 0 )
            {
                StdString::strcpy_safe( _out, _path, MENGINE_MAX_PATH );

                return;
            }

            StdString::strcpy_safe( _out, _base, MENGINE_MAX_PATH );

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::strchrcat( _out, _slash );
            }

            StdString::strcat_safe( _out, _path, MENGINE_MAX_PATH );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathAppendW( WChar * const _base, const WChar * _path, WChar _slash )
        {
            size_t base_lenght = StdString::wcslen( _base );

            if( base_lenght == 0 )
            {
                StdString::wcscpy_safe( _base, _path, MENGINE_MAX_PATH );

                return;
            }

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::wcschrcat_safe( _base, _slash, MENGINE_MAX_PATH );
            }

            StdString::wcscat_safe( _base, _path, MENGINE_MAX_PATH );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathAppendA( Char * const _base, const Char * _path, Char _slash )
        {
            size_t base_lenght = StdString::strlen( _base );

            if( base_lenght == 0 )
            {
                StdString::strcpy_safe( _base, _path, MENGINE_MAX_PATH );

                return;
            }

            if( _base[base_lenght - 1] != _slash )
            {
                StdString::strchrcat( _base, _slash );
            }

            StdString::strcat_safe( _base, _path, MENGINE_MAX_PATH );
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

            StdString::wcschrcat_safe( _folderPath, _delim, MENGINE_MAX_PATH );
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

            StdString::strchrcat_safe( _folderPath, _delim, MENGINE_MAX_PATH );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashW( WChar * const _filePath )
        {
            WChar * pch = StdString::wcschr( _filePath, MENGINE_PATH_WBACKSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_WFORWARDSLASH;

                pch = StdString::wcschr( pch, MENGINE_PATH_WBACKSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashA( Char * const _filePath )
        {
            Char * pch = StdString::strchr( _filePath, MENGINE_PATH_BACKSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_FORWARDSLASH;

                pch = StdString::strchr( pch, MENGINE_PATH_BACKSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToW( WChar * const _out, const WChar * _filePath )
        {
            StdString::wcscpy_safe( _out, _filePath, MENGINE_MAX_PATH );

            Helper::pathCorrectBackslashW( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToA( Char * const _out, const Char * _filePath )
        {
            StdString::strcpy_safe( _out, _filePath, MENGINE_MAX_PATH );

            Helper::pathCorrectBackslashA( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashW( WChar * const _filePath )
        {
            WChar * pch = StdString::wcschr( _filePath, MENGINE_PATH_WFORWARDSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_WBACKSLASH;

                pch = StdString::wcschr( pch, MENGINE_PATH_WFORWARDSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashA( Char * const _filePath )
        {
            Char * pch = StdString::strchr( _filePath, MENGINE_PATH_FORWARDSLASH );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_BACKSLASH;

                pch = StdString::strchr( pch, MENGINE_PATH_FORWARDSLASH );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashToW( WChar * const _out, const WChar * _filePath )
        {
            StdString::wcscpy_safe( _out, _filePath, MENGINE_MAX_PATH );

            Helper::pathCorrectForwardslashW( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashToA( Char * const _out, const Char * _filePath )
        {
            StdString::strcpy_safe( _out, _filePath, MENGINE_MAX_PATH );

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