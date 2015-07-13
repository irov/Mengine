#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Core/FilePath.h"
#	include "Core/ConstString.h"
#	include "Core/MemoryCacheInput.h"

#	include "Config/Typedef.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheInputPtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheInputPtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc );
	}
}