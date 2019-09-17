#include "PathHelper.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"

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
        bool makeFullPath( const ConstString & _fileGroupName, const FilePath & _filePath, FilePath & _fullPath )
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

            _fullPath = fullPath;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath getPathFolder( const FilePath & _fullpath )
        {
            const Char * str_fullpath = _fullpath.c_str();

            const Char * folder_delimiter_1 = strrchr( str_fullpath, '\\' );
            const Char * folder_delimiter_2 = strrchr( str_fullpath, '/' );

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

            const Char * folder_delimiter_1 = strrchr( str_fullpath, '\\' );
            const Char * folder_delimiter_2 = strrchr( str_fullpath, '/' );

            const Char * folder_delimiter = (folder_delimiter_1 > folder_delimiter_2) ? folder_delimiter_1 : folder_delimiter_2;

            if( folder_delimiter == nullptr )
            {
                return Helper::stringizeFilePath( _fileSpec );
            }

            FilePath::size_type size_fullpath = (FilePath::size_type)(folder_delimiter - str_fullpath + 1);

            FilePath c_folder = Helper::stringizeFilePathFormat( "%.*s%s", size_fullpath, str_fullpath, _fileSpec );

            return c_folder;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath unicodeToFilePath( const WString & _unicode )
        {
            String utf8;
            if( Helper::unicodeToUtf8Size( _unicode.c_str(), _unicode.size(), utf8 ) == false )
            {
                return FilePath( ConstString::none() );
            }

            FilePath fp = Helper::stringizeFilePath( utf8 );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashW( WChar * _filePath )
        {
            WChar * pch = ::wcschr( _filePath, L'\\' );
            while( pch != NULL )
            {
                *pch = L'/';

                pch = ::wcschr( pch, L'\\' );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToA( Char * _out, const Char * _in )
        {
            ::strcpy( _out, _in );

            pathCorrectBackslashA( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashToW( WChar * _out, const WChar * _in )
        {
            ::wcscpy( _out, _in );

            pathCorrectBackslashW( _out );
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashW( WChar * _filePath )
        {
            WChar * pch = ::wcschr( _filePath, L'/' );
            while( pch != NULL )
            {
                *pch = L'\\';

                pch = ::wcschr( pch, L'/' );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectForwardslashA( Char * _filePath )
        {
            Char * pch = ::strchr( _filePath, '/' );
            while( pch != NULL )
            {
                *pch = '\\';

                pch = ::strchr( pch, '/' );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveBackslashW( WChar * _filePath )
        {
            size_t len = ::wcslen( _filePath );

            WChar * pch = _filePath + len - 1;

            if( *pch == L'/' )
            {
                *pch = L'\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecW( WChar * _filePath )
        {
            size_t len = ::wcslen( _filePath );

            if( len == 0 )
            {
                return false;
            }

            WChar * pch = _filePath + len - 1;

            if( *pch == L'/' )
            {
                return false;
            }

            pch--;

            while( *pch != L'/' )
            {
                if( pch == _filePath )
                {
                    return false;
                }

                pch--;
            }

            pch++;

            *pch = L'\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void pathCorrectBackslashA( Char * _filePath )
        {
            Char * pch = ::strchr( _filePath, '\\' );

            while( pch != NULL )
            {
                *pch = '/';

                pch = ::strchr( pch, '\\' );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void pathRemoveBackslashA( Char * _filePath )
        {
            size_t len = ::strlen( _filePath );

            Char * pch = _filePath + len - 1;

            if( *pch == '/' )
            {
                *pch = '\0';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool pathRemoveFileSpecA( Char * _filePath )
        {
            size_t len = ::strlen( _filePath );

            if( len == 0 )
            {
                return false;
            }

            Char * pch = _filePath + len - 1;

            if( *pch == '/' )
            {
                return false;
            }

            pch--;

            while( *pch != '/' )
            {
                if( pch == _filePath )
                {
                    return false;
                }

                pch--;
            }

            pch++;

            *pch = '\0';

            return true;
        }
    }
}