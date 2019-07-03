#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"
#include "Interface/ArchivatorInterface.h"
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
        MemoryInterfacePtr loadStreamArchiveBuffer( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const Char * _doc );
        bool loadStreamArchiveInplace( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, void * _data, size_t _size, const Char * _doc );
        bool loadStreamArchiveBufferSize( const InputStreamInterfacePtr & _stream, size_t & _size );
        bool writeStreamArchiveBuffer( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
        //////////////////////////////////////////////////////////////////////////		
        MemoryInterfacePtr loadStreamArchiveData( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const Char * _doc );
        bool writeStreamArchiveData( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr loadStreamArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const Char * _doc );
        MemoryInterfacePtr loadStreamArchiveMagicMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const Char * _doc );
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr loadStreamCacheArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const Char * _doc );
    }
}