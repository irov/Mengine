#pragma once

#include "Interface/ArchiveServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class ArchiveService
        : public ServiceBase<ArchiveServiceInterface>
    {
    public:
        ArchiveService();
        ~ArchiveService() override;

    public:
        MemoryInputInterfacePtr compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, EArchivatorCompress _compress ) override;
        bool decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * const _memory, size_t _capacity, size_t * const _decompressSize ) override;

    public:
        MemoryInputInterfacePtr compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size, EArchivatorCompress _compress ) override;
        MemoryInputInterfacePtr compressBufferZ( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size, EArchivatorCompress _compress ) override;
    };
}
