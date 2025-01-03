#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EArchivatorCompress
    {
        EAC_FAST,
        EAC_NORMAL,
        EAC_BEST
    };
    //////////////////////////////////////////////////////////////////////////
    class ArchivatorInterface
        : public Mixin
    {
    public:
        virtual size_t compressBound( size_t _size ) const = 0;

    public:
        virtual bool compress( void * const _buffer, size_t _bufferCapacity, const void * _source, size_t _sourceSize, size_t * const _compressSize, EArchivatorCompress _compress ) = 0;
        virtual bool decompress( void * const _buffer, size_t _bufferCapacity, const void * _source, size_t _sourceSize, size_t * const _uncompressSize ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArchivatorInterface> ArchivatorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}