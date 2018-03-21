#   pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/StreamInterface.h"
#include "Interface/MemoryInterface.h"

#include "Factory/FactorablePtr.h"

#include "Core/Magic.h"
#include "Core/ConstString.h"

namespace Mengine
{
	enum EArchivatorCompress
	{
		EAC_FAST,
		EAC_NORMAL,
		EAC_BEST
	};

	class ArchivatorInterface
		: public ServantInterface
	{
	public:
		virtual size_t compressBound( size_t _size ) const = 0;

	public:
		virtual bool compress( void * _buffer, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize, EArchivatorCompress _compress ) = 0;
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
		virtual bool hasArchivator( const ConstString & _type ) const = 0;
        virtual ArchivatorInterfacePtr getArchivator( const ConstString & _type ) const = 0;

	public:
		virtual bool decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress ) = 0;
		virtual MemoryInputInterfacePtr compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, EArchivatorCompress _compress ) = 0;

	public:
		virtual MemoryInputInterfacePtr compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size, EArchivatorCompress _compress ) = 0;
    };

#   define ARCHIVE_SERVICE()\
    ((Mengine::ArchiveServiceInterface *)SERVICE_GET(Mengine::ArchiveServiceInterface))
}