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
        virtual bool compress( unsigned char * _buffer, size_t _bufferSize, size_t & _compressSize, const unsigned char * _source, size_t _sourceSize ) = 0;
        virtual bool uncompress( unsigned char * _buffer, size_t _bufferSize, size_t & _uncompressSize, const unsigned char * _source, size_t _sourceSize ) = 0;
    };

#   define ARCHIVE_SERVICE( serviceProvider )\
    (Menge::getService<Menge::ArchiveServiceInterface>(serviceProvider))
}