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
        virtual size_t compressBound( size_t _size ) = 0;

    public:
        virtual bool compress( void * _buffer, size_t _bufferSize, size_t & _compressSize, const void * _source, size_t _sourceSize ) = 0;
        virtual bool uncompress( void * _buffer, size_t _bufferSize, size_t & _uncompressSize, const void * _source, size_t _sourceSize ) = 0;
    };

#   define ARCHIVE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::ArchiveServiceInterface>(serviceProvider))
}