#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ArchiveServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ArchiveService" );

    public:
        virtual bool decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * const _memory, size_t _capacity, size_t * const _uncompress ) = 0;
        virtual MemoryInputInterfacePtr compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, EArchivatorCompress _compress ) = 0;

    public:
        virtual MemoryInputInterfacePtr compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size, EArchivatorCompress _compress ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ARCHIVE_SERVICE()\
    ((Mengine::ArchiveServiceInterface *)SERVICE_GET(Mengine::ArchiveServiceInterface))
//////////////////////////////////////////////////////////////////////////