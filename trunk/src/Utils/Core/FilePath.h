#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"

#	include "stdex/array_string.h"

#	include <stdex/stl_vector.h>

#	include <stdint.h>

namespace Menge
{
#   ifndef MAX_PATH
#   define MAX_PATH 260
#   endif

#	define MENGINE_FILE_PATH_INVALID_ID	((uint32_t)-1)

	typedef ConstString FilePath;

	//typedef ConstString FilePath; //UTF8
	typedef stdex::array_string<MAX_PATH> PathString;

    typedef stdex::vector<FilePath> TVectorFilePath;

    ConstString concatenationFilePath( ServiceProviderInterface * _serviceProvider, const ConstString & _left, const ConstString & _right );
}  