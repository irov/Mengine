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
        : m_serviceProvider(NULL)
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
    bool ArchiveService::compress( unsigned char * _buffer, size_t _bufferSize, size_t & _compressSize, const unsigned char * _source, size_t _sourceSize )
    {
        uLongf compressSize = _bufferSize;

        int zerr = ::compress( _buffer, &compressSize, _source, _sourceSize );

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
    bool ArchiveService::uncompress( unsigned char * _buffer, size_t _bufferSize, size_t & _uncompressSize, const unsigned char * _source, size_t _sourceSize )
    {
        uLongf destLen = _bufferSize;
        int zerr = ::uncompress( _buffer, &destLen, _source, _sourceSize );

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
