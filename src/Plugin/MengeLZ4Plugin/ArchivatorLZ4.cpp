#	include "ArchivatorLZ4.h"

#	include "Logger/Logger.h"

#   include "lz4.h"
#   include "lz4hc.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
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
		int size = ::LZ4_compressBound( (int)_size );

		size_t total_size = size;

		return total_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorLZ4::compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize )
    {
		char * dst_buffer = (char *)_distance;
		const char * src_buffer = (const char *)_source;

		int compressSize = ::LZ4_compressHC2_limitedOutput( src_buffer, dst_buffer, (int)_sourceSize, (int)_bufferSize, 16 );

		if( compressSize < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchivatorLZ4::compress invalid compress code [%d]"
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
		(void)_sourceSize;

		char * dst_buffer = (char *)_distance;
		const char * src_buffer = (const char *)_source;

		int decompressSize = ::LZ4_decompress_fast( src_buffer, dst_buffer, (int)_bufferSize );

		if( decompressSize < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchivatorLZ4::decompress invalid uncompress code [%d]"
				, decompressSize
				);

			return false;
		}

		_decompressSize = _bufferSize;

		return true;
    }
}
