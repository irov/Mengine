#pragma once

#include "Interface/Interface.h"

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
        : public Interface
    {
    public:
        virtual size_t compressBound( size_t _size ) const = 0;

    public:
        virtual bool compress( void * _buffer, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t * _compressSize, EArchivatorCompress _compress ) = 0;
        virtual bool decompress( void * _buffer, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t * _uncompressSize ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArchivatorInterface> ArchivatorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}