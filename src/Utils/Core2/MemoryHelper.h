#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Core/FilePath.h"
#include "Core/ConstString.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryStream( const InputStreamInterfacePtr & _stream, const Char * _file, uint32_t _line );
		MemoryInterfacePtr createMemoryStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _file, uint32_t _line );
		MemoryInterfacePtr createMemoryFile( const FileGroupInterfacePtr & _category, const FilePath & _filePath, bool _stream, const Char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheBuffer( size_t _size, const Char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheStream( const InputStreamInterfacePtr & _stream, const Char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheFile( const FileGroupInterfacePtr & _category, const FilePath & _filePath, bool _stream, const Char * _file, uint32_t _line );
        MemoryInterfacePtr createMemoryCacheFileString( const FileGroupInterfacePtr & _category, const FilePath & _filePath, bool _stream, const Char * _file, uint32_t _line );
	}
}	


