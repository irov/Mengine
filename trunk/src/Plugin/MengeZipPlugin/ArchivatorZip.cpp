#	include "ArchivatorZip.h"

#	include "Logger/Logger.h"

#   include "zlib.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ArchivatorZip::ArchivatorZip()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ArchivatorZip::~ArchivatorZip()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ArchivatorZip::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ArchivatorZip::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ArchivatorZip::compressBound( size_t _size ) const
    {
        uLong size = ::compressBound( _size );

		size_t total_size = size;

        return total_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorZip::compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize )
    {
        uLongf compressSize = _bufferSize;

        Bytef * dst_buffer = (Bytef *)_distance;
        const Bytef * src_buffer = (const Bytef *)_source;

        int zerr = ::compress( dst_buffer, &compressSize, src_buffer, _sourceSize );

        if( zerr != Z_OK )
        {
            LOGGER_ERROR(m_serviceProvider)("ArchivatorZip::compress invalid compress code [%d]"
                , zerr
                );

            return false;
        }

        _compressSize = compressSize;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	static voidpf my_alloc_func( voidpf opaque, uInt items, uInt size )
	{
		(void)opaque;

		return stdex_malloc_threadsafe( items * size );
	}
	//////////////////////////////////////////////////////////////////////////
	static void my_free_func( voidpf opaque, voidpf address )
	{
		(void)opaque;

		stdex_free_threadsafe( address );
	}
	//////////////////////////////////////////////////////////////////////////
	static int my_uncompress( Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen )
	{
		z_stream stream;
		int err;

		stream.next_in = const_cast<Bytef *>(source);
		stream.avail_in = (uInt)sourceLen;
		/* Check for source > 64K on 16-bit machine: */
		if( (uLong)stream.avail_in != sourceLen ) return Z_BUF_ERROR;

		stream.next_out = dest;
		stream.avail_out = (uInt)*destLen;
		if( (uLong)stream.avail_out != *destLen ) return Z_BUF_ERROR;

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
		*destLen = stream.total_out;

		err = inflateEnd( &stream );

		return Z_OK;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorZip::decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _decompressSize )
    {
        uLongf destLen = _bufferSize;

        Bytef * dst_buffer = (Bytef *)_distance;
        const Bytef * src_buffer = (const Bytef *)_source;

		int zerr = my_uncompress( dst_buffer, &destLen, src_buffer, _sourceSize );

        if( zerr != Z_OK )
        {
            LOGGER_ERROR(m_serviceProvider)("ArchivatorZip::decompress invalid uncompress code [%d]"
                , zerr
                );

            return false;
        }

        _decompressSize = destLen;

        return true;
    }
}
