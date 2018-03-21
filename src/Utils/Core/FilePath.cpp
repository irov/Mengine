#   include "FilePath.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"

#include "Core/FilePath.h"

#include "Logger/Logger.h"

namespace Mengine
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		FilePath emptyPath()
		{
			return FilePath( ConstString::none() );
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath concatenationFilePath( const FilePath & _left, const FilePath & _right )
		{
			PathString path;

			path += _left;
			path += _right;

			ConstString c_path = Helper::stringizeStringSize( path.c_str(), path.size() );

			return FilePath(c_path);
		}
		//////////////////////////////////////////////////////////////////////////
		bool makeFullPath( const ConstString & _fileGroupName, const FilePath & _fileName, FilePath & _fullPath )
		{
			FileGroupInterfacePtr group;

			if( FILE_SERVICE()
				->hasFileGroup( _fileGroupName, &group ) == false )
			{
                LOGGER_ERROR( "makeFullPath: not found file group '%s'"
					, _fileGroupName.c_str() 
					);

				return false;
			}

			const FilePath & groupPath = group->getFolderPath();

			FilePath fullPath = Helper::concatenationFilePath( groupPath, _fileName );

			_fullPath = fullPath;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath getPathFolder( const FilePath & _fullpath )
		{
			const Char * str_fullpath = _fullpath.c_str();

			const Char * folder_delimiter_1 = strrchr( str_fullpath, '\\');
			const Char * folder_delimiter_2 = strrchr( str_fullpath, '/' );

			const Char * folder_delimiter = (folder_delimiter_1 > folder_delimiter_2) ? folder_delimiter_1 : folder_delimiter_2;

			if( folder_delimiter == nullptr )
			{
				return FilePath(ConstString::none());
			}

			FilePath c_folder = Helper::stringizeFilePath( str_fullpath, folder_delimiter - str_fullpath + 1 );

			return c_folder;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath stringizeFilePath( const Char * _value, ConstString::size_type _size )
		{
			ConstString cstr = stringizeStringSize( _value, _size );

			return FilePath( cstr );
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath stringizeFilePath( const String & _path )
		{
			FilePath fp = stringizeFilePath( _path.c_str(), _path.size() );

			return fp;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath stringizeFilePath( const PathString & _path )
		{
			FilePath fp = stringizeFilePath( _path.c_str(), _path.size() );

			return fp;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath unicodeToFilePath( const WString & _unicode )
		{
			String utf8;
			if( Helper::unicodeToUtf8Size( _unicode.c_str(), _unicode.size(), utf8 ) == false )
			{
				return FilePath(ConstString::none());
			}

			FilePath fp = Helper::stringizeFilePath( utf8 );

			return fp;
		}

		//////////////////////////////////////////////////////////////////////////
		void pathCorrectBackslash( WChar * _out, const WChar * _in )
		{
			wcscpy( _out, _in );

			WChar * pch = wcschr( _out, L'\\' );
			while( pch != NULL )
			{
				*pch = L'/';

				pch = wcschr( pch + 1, L'\\' );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void pathRemoveBackslash( WChar * _path )
		{
			size_t len = wcslen( _path );

			WChar * pch = _path + len - 1;

			if( *pch == L'/' )
			{
				*pch = L'\0';
			}
		}
		//////////////////////////////////////////////////////////////////////////
		bool pathRemoveFileSpec( WChar * _path )
		{
			size_t len = wcslen( _path );

            if( len == 0 )
            {
                return true;
            }

			WChar * pch = _path + len - 1;

			if( *pch == L'/' )
			{
				return false;
			}

			pch--;

			while( *pch != L'/' )
			{
				pch--;
			}

			pch++;

			*pch = L'\0';

			return true;
		}
	}
}  