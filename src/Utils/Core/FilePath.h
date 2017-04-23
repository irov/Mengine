#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Core/ConstString.h"

#	include "Config/String.h"

#	include "stdex/array_string.h"

#	include <stdex/stl_vector.h>

#	include <stdint.h>

namespace Menge
{
#   ifndef MENGINE_MAX_PATH
#   define MENGINE_MAX_PATH 260
#   endif

#	define MENGINE_FILE_PATH_INVALID_ID	((uint32_t)-1)

	class FilePath
		: public ConstString
	{
	public:
		FilePath() 
		{
		}

		explicit FilePath( const ConstString & _cst )
			: ConstString( _cst )
		{
		}
	};

	//typedef ConstString FilePath; //UTF8
	typedef stdex::array_string<MENGINE_MAX_PATH> PathString;

    typedef stdex::vector<FilePath> TVectorFilePath;

	namespace Helper
	{
		FilePath concatenationFilePath( ServiceProviderInterface * _serviceProvider, const FilePath & _left, const FilePath & _right );
		
		bool makeFullPath( ServiceProviderInterface * _serviceProvider, const ConstString & _fileGroupName, const FilePath & _fileName, FilePath & _fullPath );
		FilePath getPathFolder( ServiceProviderInterface * _serviceProvider, const FilePath & _fullpath );

		//////////////////////////////////////////////////////////////////////////
		FilePath stringizeFilePath( ServiceProviderInterface * _serviceProvider, const Char * _value, ConstString::size_type _size );
		FilePath stringizeFilePath( ServiceProviderInterface * _serviceProvider, const String & _path );
		FilePath stringizeFilePath( ServiceProviderInterface * _serviceProvider, const PathString & _path );
	}

#	define STRINGIZE_FILEPATH_LOCAL( serviceProvider, str )\
	Helper::stringizeFilePath( serviceProvider, str, (sizeof(str) - 1) )
}  