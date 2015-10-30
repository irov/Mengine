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
		MemoryInterfacePtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream );
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size );
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheFileString( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc );
	}
}	// namespace Menge


