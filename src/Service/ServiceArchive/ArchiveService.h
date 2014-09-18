#	pragma once

#	include "Interface/ArchiveInterface.h"

#	include "Factory/FactoryStore.h"

#	include "stdex/binary_vector.h"

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
		void registerArchivator( const ConstString & _type, const ArchivatorInterfacePtr & _archivator ) override;
		void unregisterArchivator( const ConstString & _type ) override;

	public:		
		ArchivatorInterfacePtr getArchivator( const ConstString & _type ) const override;

	public:
		bool decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress ) override;
		MemoryInputPtr compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream ) override;
			
	public:
		MemoryInputPtr compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef stdex::binary_vector<ConstString, ArchivatorInterfacePtr> TMapArchivators;
		TMapArchivators m_archivators;
	};
}
