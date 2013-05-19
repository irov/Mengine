#	pragma once

#	include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ArchiveService
		: public ArchiveServiceInterface
	{
	public:
		ArchiveService();
		~ArchiveService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;
        
    public:
        size_t compressBound( size_t _size ) override;

    public:
        bool compress( unsigned char * _buffer, size_t _bufferSize, size_t & _compressSize, const unsigned char * _source, size_t _sourceSize ) override;
        bool uncompress( unsigned char * _buffer, size_t _bufferSize, size_t & _uncompressSize, const unsigned char * _source, size_t _sourceSize ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}
