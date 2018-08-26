#pragma once

#include "Interface/ArchiveInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Map.h"

namespace Mengine
{
    class ArchiveService
        : public ServiceBase<ArchiveServiceInterface>
    {
    public:
        ArchiveService();
        ~ArchiveService() override;

    public:
        void registerArchivator( const ConstString & _type, const ArchivatorInterfacePtr & _archivator ) override;
        void unregisterArchivator( const ConstString & _type ) override;

    public:
        bool hasArchivator( const ConstString & _type ) const override;
        const ArchivatorInterfacePtr & getArchivator( const ConstString & _type ) const override;

    public:
        bool decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress ) override;
        MemoryInputInterfacePtr compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, EArchivatorCompress _compress ) override;

    public:
        MemoryInputInterfacePtr compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size, EArchivatorCompress _compress ) override;

    protected:
        typedef Map<ConstString, ArchivatorInterfacePtr> MapArchivators;
        MapArchivators m_archivators;
    };
}
