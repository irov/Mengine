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
		MemoryInterfacePtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _file, uint32_t _line );
		MemoryInterfacePtr createMemoryStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _file, uint32_t _line );
		MemoryInterfacePtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheFileString( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _file, uint32_t _line );
	}
}	// namespace Menge


