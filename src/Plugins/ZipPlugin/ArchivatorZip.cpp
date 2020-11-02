#include "ArchivatorZip.h"

#include "Kernel/Logger.h"
#include "Kernel/AllocatorHelper.h"

#include "zlib.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArchivatorZip::ArchivatorZip()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArchivatorZip::~ArchivatorZip()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ArchivatorZip::compressBound( size_t _size ) const
    {
        uLong size = ::compressBound( (uLong)_size );

        return (size_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorZip::compress( void * const _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t * const _compressSize, EArchivatorCompress _compress )
    {
        uLong compressSize = (uLong)_bufferSize;

        Bytef * dst_buffer = static_cast<Bytef *>(_distance);
        const Bytef * src_buffer = static_cast<const Bytef *>(_source);

        int32_t compress_method = 0;

        switch( _compress )
        {
        case EAC_FAST:
            {
                compress_method = 1;
            }break;
        case EAC_NORMAL:
            {
                compress_method = 6;
            }break;
        case EAC_BEST:
            {
                compress_method = 9;
            }
        };

        int32_t zerr = ::compress2( dst_buffer, &compressSize, src_buffer, (uLong)_sourceSize, compress_method );

        if( zerr != Z_OK )
        {
            LOGGER_ERROR( "invalid compress code [%d]"
                , zerr
            );

            return false;
        }

        *_compressSize = (size_t)compressSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static voidpf my_alloc_func( voidpf _opaque, uInt _items, uInt _size )
    {
        MENGINE_UNUSED( _opaque );

        uInt total = _items * _size;

        void * p = Helper::allocateMemory( total, "zip" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_free_func( voidpf _opaque, voidpf _address )
    {
        MENGINE_UNUSED( _opaque );

        Helper::deallocateMemory( _address, "zip" );
    }
    //////////////////////////////////////////////////////////////////////////
    static int32_t my_uncompress( Bytef * _dest, uLong * _destLen, const Bytef * _source, uLong _sourceLen )
    {
        z_stream stream;
        int32_t err;

        stream.next_in = const_cast<Bytef *>(_source);
        stream.avail_in = (uInt)_sourceLen;
        /* Check for source > 64K on 16-bit machine: */
        if( (uLong)stream.avail_in != _sourceLen ) return Z_BUF_ERROR;

        stream.next_out = _dest;
        stream.avail_out = (uInt)* _destLen;
        if( (uLong)stream.avail_out != *_destLen ) return Z_BUF_ERROR;

        stream.zalloc = (alloc_func)my_alloc_func;
        stream.zfree = (free_func)my_free_func;

        err = inflateInit( &stream );
        if( err != Z_OK ) return err;

        err = inflate( &stream, Z_FINISH );
        if( err != Z_STREAM_END )
        {
            inflateEnd( &stream );
            if( err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0) )
                return Z_DATA_ERROR;
            return err;
        }
        *_destLen = stream.total_out;

        err = inflateEnd( &stream );

        return err;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorZip::decompress( void * const _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t * const _decompressSize )
    {
        uLong destLen = (uLong)_bufferSize;

        Bytef * dst_buffer = (Bytef *)_distance;
        const Bytef * src_buffer = (const Bytef *)_source;

        int32_t zerr = my_uncompress( dst_buffer, &destLen, src_buffer, (uLong)_sourceSize );

        if( zerr != Z_OK )
        {
            LOGGER_ERROR( "invalid uncompress code [%d]"
                , zerr
            );

            return false;
        }

        *_decompressSize = (size_t)destLen;

        return true;
    }
}
