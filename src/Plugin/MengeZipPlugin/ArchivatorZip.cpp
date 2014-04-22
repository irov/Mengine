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
    bool ArchivatorZip::decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _uncompressSize )
    {
        uLongf destLen = _bufferSize;

        Bytef * dst_buffer = (Bytef *)_distance;
        const Bytef * src_buffer = (const Bytef *)_source;

        int zerr = ::uncompress( dst_buffer, &destLen, src_buffer, _sourceSize );

        if( zerr != Z_OK )
        {
            LOGGER_ERROR(m_serviceProvider)("ArchivatorZip::decompress invalid uncompress code [%d]"
                , zerr
                );

            return false;
        }

        _uncompressSize = destLen;

        return true;
    }
}
