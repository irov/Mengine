#	pragma once

#	include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ArchivatorLZ4
		: public ArchivatorInterface
	{
	public:
		ArchivatorLZ4();
		~ArchivatorLZ4();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;
        
    public:
        size_t compressBound( size_t _size ) const override;

    public:
        bool compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize ) override;
        bool decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _decompressSize ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}
