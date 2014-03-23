#   pragma once

#	include "Config/Typedef.h"

#   include "Interface/ServiceInterface.h"

namespace Menge
{
    class ArchiveServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ArchiveService" )

    public:
        virtual size_t compressBound( size_t _size ) const = 0;

    public:
        virtual bool compress( void * _buffer, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize ) = 0;
        virtual bool decompress( void * _buffer, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _uncompressSize ) = 0;
    };

#   define ARCHIVE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::ArchiveServiceInterface>(serviceProvider))
}