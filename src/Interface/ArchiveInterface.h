#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/StreamInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Core/Magic.h"
#	include "Core/ConstString.h"
#	include "Core/MemoryInput.h"

namespace Menge
{
	class ArchivatorInterface
		: public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

	public:
		virtual size_t compressBound( size_t _size ) const = 0;

	public:
		virtual bool compress( void * _buffer, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize ) = 0;
		virtual bool decompress( void * _buffer, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _uncompressSize ) = 0;
	};

	typedef stdex::intrusive_ptr<ArchivatorInterface> ArchivatorInterfacePtr;

    class ArchiveServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ArchiveService" )

    public:
        virtual void registerArchivator( const ConstString & _type, const ArchivatorInterfacePtr & _archivator ) = 0;
		virtual void unregisterArchivator( const ConstString & _type ) = 0;

    public:
        virtual ArchivatorInterfacePtr getArchivator( const ConstString & _type ) const = 0;

	public:
		virtual bool decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress ) = 0;
		virtual MemoryInputPtr compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream ) = 0;

	public:
		virtual MemoryInputPtr compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size ) = 0;
    };

#   define ARCHIVE_SERVICE( serviceProvider )\
    ((Menge::ArchiveServiceInterface *)SERVICE_GET(serviceProvider, Menge::ArchiveServiceInterface))
}