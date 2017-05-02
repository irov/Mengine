#   include "FilePath.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "Core/FilePath.h"

#	include "Logger/Logger.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		FilePath concatenationFilePath( ServiceProviderInterface * _serviceProvider, const FilePath & _left, const FilePath & _right )
		{
			PathString path;

			path += _left;
			path += _right;

			ConstString c_path = Helper::stringizeStringSize( _serviceProvider, path.c_str(), path.size() );

			return FilePath(c_path);
		}
		//////////////////////////////////////////////////////////////////////////
		bool makeFullPath( ServiceProviderInterface * _serviceProvider, const ConstString & _fileGroupName, const FilePath & _fileName, FilePath & _fullPath )
		{
			FileGroupInterfacePtr group;

			if( FILE_SERVICE(_serviceProvider)
				->hasFileGroup( _fileGroupName, &group ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("makeFullPath: not found file group '%s'"
					, _fileGroupName.c_str() 
					);

				return false;
			}

			const FilePath & groupPath = group->getPath();

			FilePath fullPath = Helper::concatenationFilePath( _serviceProvider, groupPath, _fileName );

			_fullPath = fullPath;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath getPathFolder( ServiceProviderInterface * _serviceProvider, const FilePath & _fullpath )
		{
			const Char * str_fullpath = _fullpath.c_str();

			const Char * folder_delimiter_1 = strrchr( str_fullpath, '\\');
			const Char * folder_delimiter_2 = strrchr( str_fullpath, '/' );

			const Char * folder_delimiter = (folder_delimiter_1 > folder_delimiter_2) ? folder_delimiter_1 : folder_delimiter_2;

			if( folder_delimiter == nullptr )
			{
				return FilePath(ConstString::none());
			}

			FilePath c_folder = Helper::stringizeFilePath( _serviceProvider, str_fullpath, folder_delimiter - str_fullpath + 1 );

			return c_folder;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath stringizeFilePath( ServiceProviderInterface * _serviceProvider, const Char * _value, ConstString::size_type _size )
		{
			ConstString cstr = stringizeStringSize( _serviceProvider, _value, _size );

			return FilePath( cstr );
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath stringizeFilePath( ServiceProviderInterface * _serviceProvider, const String & _path )
		{
			FilePath fp = stringizeFilePath( _serviceProvider, _path.c_str(), _path.size() );

			return fp;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath stringizeFilePath( ServiceProviderInterface * _serviceProvider, const PathString & _path )
		{
			FilePath fp = stringizeFilePath( _serviceProvider, _path.c_str(), _path.size() );

			return fp;
		}
		//////////////////////////////////////////////////////////////////////////
		FilePath unicodeToFilePath( ServiceProviderInterface * _serviceProvide, const WString & _unicode )
		{
			String utf8;
			if( Helper::unicodeToUtf8Size( _serviceProvide, _unicode.c_str(), _unicode.size(), utf8 ) == false )
			{
				return FilePath(ConstString::none());
			}

			FilePath fp = Helper::stringizeFilePath( _serviceProvide, utf8 );

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