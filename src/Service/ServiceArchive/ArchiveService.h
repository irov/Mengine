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
        bool compress( void * _distance, size_t _bufferSize, size_t & _compressSize, const void * _source, size_t _sourceSize ) override;
        bool uncompress( void * _distance, size_t _bufferSize, size_t & _uncompressSize, const void * _source, size_t _sourceSize ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}
