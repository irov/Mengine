#pragma once

#include "Interface/ArchivatorInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ArchivatorLZ4
        : public ArchivatorInterface
        , public Factorable
    {
    public:
        ArchivatorLZ4();
        ~ArchivatorLZ4() override;

    public:
        size_t compressBound( size_t _size ) const override;

    public:
        bool compress( void * const _distance, size_t _bufferCapacity, const void * _source, size_t _sourceSize, size_t * const _compressSize, EArchivatorCompress _compress ) override;
        bool decompress( void * const _distance, size_t _bufferCapacity, const void * _source, size_t _sourceSize, size_t * const _decompressSize ) override;
    };
}
