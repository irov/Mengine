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
		MemoryInterfacePtr createMemoryStream( const InputStreamInterfacePtr & _stream, const char * _file, uint32_t _line );
		MemoryInterfacePtr createMemoryStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const char * _file, uint32_t _line );
		MemoryInterfacePtr createMemoryFile( const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheBuffer( size_t _size, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheStream( const InputStreamInterfacePtr & _stream, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheFile( const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheFileString( const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _file, uint32_t _line );
	}
}	// namespace Menge


