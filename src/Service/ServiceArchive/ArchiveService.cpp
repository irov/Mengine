#	include "ArchiveService.h"

#	include "Logger/Logger.h"

#   include "zlib.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ArchiveService, Menge::ArchiveServiceInterface, Menge::ArchiveService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ArchiveService::ArchiveService()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ArchiveService::~ArchiveService()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ArchiveService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ArchiveService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ArchiveService::compressBound( size_t _size )
    {
        uLong size = ::compressBound( _size );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchiveService::compress( void * _distance, size_t _bufferSize, size_t & _compressSize, const void * _source, size_t _sourceSize )
    {
        uLongf compressSize = _bufferSize;

        Bytef * z_dst_buffer = (Bytef *)_distance;
        Bytef * z_src_buffer = (Bytef *)_source;

        int zerr = ::compress( z_dst_buffer, &compressSize, z_src_buffer, _sourceSize );

        if( zerr != Z_OK )
        {
            LOGGER_ERROR(m_serviceProvider)("ArchiveService::compress invalid compress code [%d]"
                , zerr
                );

            return false;
        }

        _compressSize = compressSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchiveService::uncompress( void * _distance, size_t _bufferSize, size_t & _uncompressSize, const void * _source, size_t _sourceSize )
    {
        uLongf destLen = _bufferSize;

        Bytef * z_dst_buffer = (Bytef *)_distance;
        Bytef * z_src_buffer = (Bytef *)_source;

        int zerr = ::uncompress( z_dst_buffer, &destLen, z_src_buffer, _sourceSize );

        if( zerr != Z_OK )
        {
            LOGGER_ERROR(m_serviceProvider)("ArchiveService::uncompress invalid uncompress code [%d]"
                , zerr
                );

            return false;
        }

        _uncompressSize = destLen;

        return true;
    }
}
