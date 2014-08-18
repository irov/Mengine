#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

namespace Menge
{
	class MemoryCacheInput
		: public InputStreamInterface
	{
	public:
		MemoryCacheInput();
		~MemoryCacheInput();
		
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        void * cacheMemory( size_t _size, const char * _doc );

	public:
		size_t read( void * _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		bool skip( size_t _pos ) override;
		size_t tell() const override;
        size_t size() const override;
		bool eof() const override;

    public:
		bool time( uint64_t & _time ) const override;

	protected:
		void uncache_();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		size_t m_bufferId;

		unsigned char * m_data;
		size_t m_size;

		unsigned char * m_pos;
		unsigned char * m_end;		
	};

	typedef stdex::intrusive_ptr<MemoryCacheInput> MemoryCacheInputPtr;
}	// namespace Menge


