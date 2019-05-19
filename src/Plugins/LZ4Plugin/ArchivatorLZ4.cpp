#include "ArchivatorLZ4.h"

#include "Kernel/Logger.h"

#include "lz4.h"
#include "lz4hc.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArchivatorLZ4::ArchivatorLZ4()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArchivatorLZ4::~ArchivatorLZ4()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ArchivatorLZ4::compressBound( size_t _size ) const
    {
        int32_t size = ::LZ4_compressBound( (int)_size );

        size_t total_size = size;

        return total_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorLZ4::compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize, EArchivatorCompress _compress )
    {
        char * dst_buffer = (char *)_distance;
        const char * src_buffer = (const char *)_source;

        int32_t compress_method = 0;

        switch( _compress )
        {
        case EAC_FAST:
            {
                compress_method = LZ4HC_CLEVEL_MIN;
            }break;
        case EAC_NORMAL:
            {
                compress_method = LZ4HC_CLEVEL_DEFAULT;
            }break;
        case EAC_BEST:
            {
                compress_method = LZ4HC_CLEVEL_MAX;
            }
        };

        int32_t compressSize = ::LZ4_compress_HC( src_buffer, dst_buffer, (int32_t)_sourceSize, (int32_t)_bufferSize, compress_method );

        if( compressSize < 0 )
        {
            LOGGER_ERROR( "ArchivatorLZ4::compress invalid compress code [%d]"
                , compressSize
            );

            return false;
        }

        _compressSize = (size_t)compressSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorLZ4::decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _decompressSize )
    {
        MENGINE_UNUSED( _sourceSize );

        char * dst_buffer = (char *)_distance;
        const char * src_buffer = (const char *)_source;

        int32_t readBytes = ::LZ4_decompress_fast( src_buffer, dst_buffer, (int32_t)_bufferSize );

        if( readBytes < 0 )
        {
            LOGGER_ERROR( "invalid uncompress code [%d]"
                , readBytes
            );

            return false;
        }

        _decompressSize = _bufferSize;

        return true;
    }
}
