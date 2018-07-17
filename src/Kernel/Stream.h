#pragma once

#include "Interface/StreamInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Magic.h"

namespace Mengine
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamMagicHeader( const InputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version );
		bool writeStreamMagicHeader( const OutputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version );
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveBuffer( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, MemoryInterfacePtr & _binaryBuffer, const char * _file, uint32_t _line );
		bool loadStreamArchiveInplace( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, void * _data, size_t _size, const char * _file, uint32_t _line );
		bool loadStreamArchiveBufferSize( const InputStreamInterfacePtr & _stream, size_t & _size );
		bool writeStreamArchiveBuffer( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
		//////////////////////////////////////////////////////////////////////////		
		bool loadStreamArchiveData( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, MemoryInterfacePtr & _binaryBuffer, const char * _file, uint32_t _line );
		bool writeStreamArchiveData( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr loadStreamArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const char * _file, uint32_t _line );
		MemoryInterfacePtr loadStreamArchiveMagicMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const char * _file, uint32_t _line );
	}
}