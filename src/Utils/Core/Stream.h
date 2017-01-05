#	pragma once

#	include "Interface/StreamInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Core/Magic.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamMagicHeader( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version );
		bool writeStreamMagicHeader( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version );
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveBuffer( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, MemoryCacheBufferInterfacePtr & _binaryBuffer );
		bool loadStreamArchiveInplace( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, void * _data, size_t _size );
		bool loadStreamArchiveBufferSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t & _size );
		bool writeStreamArchiveBuffer( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
		//////////////////////////////////////////////////////////////////////////		
		bool loadStreamArchiveData( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, MemoryCacheBufferInterfacePtr & _binaryBuffer );
		bool writeStreamArchiveData( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr loadStreamArchiveMemory( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator );
		MemoryInterfacePtr loadStreamArchiveMemory( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version );
	}
}