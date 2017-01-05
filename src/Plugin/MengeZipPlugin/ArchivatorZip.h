#	pragma once

#	include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ArchivatorZip
		: public ArchivatorInterface
	{
	public:
		ArchivatorZip();
		~ArchivatorZip();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;
        
    public:
        size_t compressBound( size_t _size ) const override;

    public:
		bool compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize, EArchivatorCompress _compress ) override;
        bool decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _uncompressSize ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}
