#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Magic.h"
#include "Kernel/Data.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void readStreamData( const InputStreamInterfacePtr & _stream, Data * const _data );
        void writeStreamData( const OutputStreamInterfacePtr & _stream, const Data & _data );
        //////////////////////////////////////////////////////////////////////////
        bool readStreamMagicHeader( const InputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version );
        bool writeStreamMagicHeader( const OutputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version );
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamArchiveBuffer( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const DocumentInterfacePtr & _doc );
        bool readStreamArchiveInplace( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, void * const _data, size_t _capacity, size_t * const _size, const DocumentInterfacePtr & _doc );
        bool readStreamArchiveBufferSize( const InputStreamInterfacePtr & _stream, size_t * const _size );
        bool writeStreamArchiveBuffer( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamArchiveMagic( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const DocumentInterfacePtr & _doc );
        bool writeStreamArchiveMagic( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress );
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const DocumentInterfacePtr & _doc );
        MemoryInterfacePtr readStreamArchiveMagicMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamCacheArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
    }
}