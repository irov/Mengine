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
            if( Helper::concatenateFilePath( std::forward<InitializerList<FilePath> &&>( _paths ), concatenatePath ) == false )
            {
                return FilePath::none();
            }

            FilePath fp = Helper::stringizeFilePath( concatenatePath );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath getPathFolder( const FilePath & _fullpath )
        {
            const Char * str_fullpath = _fullpath.c_str();

            const Char * folder_delimiter_1 = StdString::strrchr( str_fullpath, MENGINE_WIN32_PATH_DELIM );
            const Char * folder_delimiter_2 = StdString::strrchr( str_fullpath, MENGINE_PATH_DELIM );

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

            const Char * folder_delimiter_1 = StdString::strrchr( str_fullpath, '\\' );
            const Char * folder_delimiter_2 = StdString::strrchr( str_fullpath, '/' );

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
        void pathCorrectBackslashW( WChar * const _filePath )
        {
            WChar * pch = StdString::wcschr( _filePath, MENGINE_WIN32_PATH_WDELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_WDELIM;

                pch = StdString::wcschr( pch, MENGINE_WIN32_PATH_WDELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToA( Char * const _out, const Char * _in )
        {
            StdString::strcpy( _out, _in );

            Helper::pathCorrectBackslashA( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToW( WChar * const _out, const WChar * _in )
        {
            StdString::wcscpy( _out, _in );

            Helper::pathCorrectBackslashW( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashW( WChar * const _filePath )
        {
            WChar * pch = StdString::wcschr( _filePath, MENGINE_PATH_WDELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_WIN32_PATH_WDELIM;

                pch = StdString::wcschr( pch, MENGINE_PATH_WDELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashA( Char * const _filePath )
        {
            Char * pch = StdString::strchr( _filePath, MENGINE_PATH_DELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_WIN32_PATH_DELIM;

                pch = StdString::strchr( pch, MENGINE_PATH_DELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveBackslashW( WChar * const _filePath )
        {
            size_t len = StdString::wcslen( _filePath );

            WChar * pch = _filePath + len - 1;

            if( *pch == MENGINE_PATH_WDELIM )
            {
                *pch = L'\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecW( WChar * const _filePath )
        {
            size_t len = StdString::wcslen( _filePath );

            if( len == 0 )
            {
                return false;
            }

            WChar * pch = _filePath + len - 1;

            if( *pch == MENGINE_PATH_WDELIM )
            {
                return false;
            }

            pch--;

            while( *pch != MENGINE_PATH_WDELIM )
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
        void pathCorrectBackslashA( Char * const _filePath )
        {
            Char * pch = StdString::strchr( _filePath, MENGINE_WIN32_PATH_DELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_DELIM;

                pch = StdString::strchr( pch, MENGINE_WIN32_PATH_DELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveBackslashA( Char * const _filePath )
        {
            size_t len = StdString::strlen( _filePath );

            Char * pch = _filePath + len - 1;

            if( *pch == MENGINE_PATH_DELIM )
            {
                *pch = '\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecA( Char * const _filePath )
        {
            size_t len = StdString::strlen( _filePath );

            if( len == 0 )
            {
                return false;
            }

            Char * pch = _filePath + len - 1;

            if( *pch == MENGINE_PATH_DELIM )
            {
                return false;
            }

            pch--;

            while( *pch != MENGINE_PATH_DELIM )
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