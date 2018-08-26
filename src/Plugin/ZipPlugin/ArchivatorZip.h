#pragma once

#include "Interface/ArchiveInterface.h"

#include "Kernel/ServantBase.h"

namespace Mengine
{
    class ArchivatorZip
        : public ServantBase<ArchivatorInterface>
    {
    public:
        ArchivatorZip();
        ~ArchivatorZip() override;

    public:
        size_t compressBound( size_t _size ) const override;

    public:
        bool compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize, EArchivatorCompress _compress ) override;
        bool decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _uncompressSize ) override;
    };
}
