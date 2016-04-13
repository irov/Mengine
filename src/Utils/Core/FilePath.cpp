#   include "FilePath.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		ConstString concatenationFilePath( ServiceProviderInterface * _serviceProvider, const ConstString & _left, const ConstString & _right )
		{
			PathString path;

			path += _left;
			path += _right;

			ConstString c_path = Helper::stringizeString( _serviceProvider, path );

			return c_path;
		}
		//////////////////////////////////////////////////////////////////////////
		bool makeFullPath( ServiceProviderInterface * _serviceProvider, const ConstString & _fileGroupName, const ConstString & _fileName, ConstString & _fullPath )
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

			const ConstString & groupPath = group->getPath();

			ConstString fullPath = Helper::concatenationFilePath( _serviceProvider, groupPath, _fileName );

			_fullPath = fullPath;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		ConstString getPathFolder( ServiceProviderInterface * _serviceProvider, const FilePath & _fullpath )
		{
			const Char * str_fullpath = _fullpath.c_str();

			const Char * folder_delimiter_1 = strrchr( str_fullpath, '\\');
			const Char * folder_delimiter_2 = strrchr( str_fullpath, '/' );

			const Char * folder_delimiter = (folder_delimiter_1 > folder_delimiter_2) ? folder_delimiter_1 : folder_delimiter_2;

			if( folder_delimiter == nullptr )
			{
				return ConstString::none();
			}

			ConstString c_folder = Helper::stringizeStringSize( _serviceProvider, str_fullpath, folder_delimiter - str_fullpath + 1 );

			return c_folder;
		}
	}
}  