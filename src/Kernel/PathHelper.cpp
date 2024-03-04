#include "PathHelper.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileServiceInterface.h"

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
        FilePath concatenationFilePath( const FilePath & _left, const FilePath & _right )
        {
            PathString path;

            path += _left;
            path += _right;

            ConstString c_path = Helper::stringizeStringSize( path.c_str(), path.size() );

            return FilePath( c_path );
        }
        //////////////////////////////////////////////////////////////////////////
        bool makeFullPath( const ConstString & _fileGroupName, const FilePath & _filePath, FilePath * const _fullPath )
        {
            FileGroupInterfacePtr group;

            if( FILE_SERVICE()
                ->hasFileGroup( _fileGroupName, &group ) == false )
            {
                LOGGER_ERROR( "not found file group '%s'"
                    , _fileGroupName.c_str()
                );

                return false;
            }

            const FilePath & groupPath = group->getFolderPath();

            FilePath fullPath = Helper::concatenationFilePath( groupPath, _filePath );

            *_fullPath = fullPath;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath getPathFolder( const FilePath & _fullpath )
        {
            const Char * str_fullpath = _fullpath.c_str();

            const Char * folder_delimiter_1 = MENGINE_STRRCHR( str_fullpath, MENGINE_WIN32_PATH_DELIM );
            const Char * folder_delimiter_2 = MENGINE_STRRCHR( str_fullpath, MENGINE_PATH_DELIM );

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

            const Char * folder_delimiter_1 = MENGINE_STRRCHR( str_fullpath, '\\' );
            const Char * folder_delimiter_2 = MENGINE_STRRCHR( str_fullpath, '/' );

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
            WChar * pch = MENGINE_WCSCHR( _filePath, MENGINE_WIN32_PATH_WDELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_WDELIM;

                pch = MENGINE_WCSCHR( pch, MENGINE_WIN32_PATH_WDELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToA( Char * const _out, const Char * _in )
        {
            MENGINE_STRCPY( _out, _in );

            Helper::pathCorrectBackslashA( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToW( WChar * const _out, const WChar * _in )
        {
            MENGINE_WCSCPY( _out, _in );

            Helper::pathCorrectBackslashW( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashW( WChar * const _filePath )
        {
            WChar * pch = MENGINE_WCSCHR( _filePath, MENGINE_PATH_WDELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_WIN32_PATH_WDELIM;

                pch = MENGINE_WCSCHR( pch, MENGINE_PATH_WDELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashA( Char * const _filePath )
        {
            Char * pch = MENGINE_STRCHR( _filePath, MENGINE_PATH_DELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_WIN32_PATH_DELIM;

                pch = MENGINE_STRCHR( pch, MENGINE_PATH_DELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveBackslashW( WChar * const _filePath )
        {
            size_t len = MENGINE_WCSLEN( _filePath );

            WChar * pch = _filePath + len - 1;

            if( *pch == MENGINE_PATH_WDELIM )
            {
                *pch = L'\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecW( WChar * const _filePath )
        {
            size_t len = MENGINE_WCSLEN( _filePath );

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
            Char * pch = MENGINE_STRCHR( _filePath, MENGINE_WIN32_PATH_DELIM );

            while( pch != nullptr )
            {
                *pch = MENGINE_PATH_DELIM;

                pch = MENGINE_STRCHR( pch, MENGINE_WIN32_PATH_DELIM );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveBackslashA( Char * const _filePath )
        {
            size_t len = MENGINE_STRLEN( _filePath );

            Char * pch = _filePath + len - 1;

            if( *pch == MENGINE_PATH_DELIM )
            {
                *pch = '\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecA( Char * const _filePath )
        {
            size_t len = MENGINE_STRLEN( _filePath );

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