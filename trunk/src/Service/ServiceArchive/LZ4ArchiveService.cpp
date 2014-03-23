#	include "LZ4ArchiveService.h"

#	include "Logger/Logger.h"

#   include "lz4.h"
#   include "lz4hc.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ArchiveService, Menge::ArchiveServiceInterface, Menge::LZ4ArchiveService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LZ4ArchiveService::LZ4ArchiveService()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	LZ4ArchiveService::~LZ4ArchiveService()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void LZ4ArchiveService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * LZ4ArchiveService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t LZ4ArchiveService::compressBound( size_t _size ) const
    {
        int size = ::LZ4_compressBound( (int)_size );

		size_t total_size = size;

        return total_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LZ4ArchiveService::compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize )
    {
        char * dst_buffer = (char *)_distance;
        const char * src_buffer = (const char *)_source;

        int compressSize = ::LZ4_compressHC2_limitedOutput( src_buffer, dst_buffer, _sourceSize, _bufferSize, 16 );

        if( compressSize < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ArchiveService::compress invalid compress code [%d]"
                , compressSize
                );

            return false;
        }

        _compressSize = (size_t)compressSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LZ4ArchiveService::decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _decompressSize )
    {
		(void)_sourceSize;

        char * dst_buffer = (char *)_distance;
        const char * src_buffer = (const char *)_source;

        int decompressSize = ::LZ4_decompress_fast( src_buffer, dst_buffer, _bufferSize );

        if( decompressSize < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("LZ4ArchiveService::decompress invalid uncompress code [%d]"
                , decompressSize
                );

            return false;
        }

        _decompressSize = _bufferSize;

        return true;
    }
}
