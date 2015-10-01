#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Core/FilePath.h"
#	include "Core/ConstString.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryFileString( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc );
	}
}	// namespace Menge


