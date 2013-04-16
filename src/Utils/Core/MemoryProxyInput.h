#	pragma once

#	include "Interface/StreamInterface.h"

namespace Menge
{
	class MemoryProxyInput
		: public InputStreamInterface
	{
	public:
		MemoryProxyInput();
		~MemoryProxyInput();

    public:
        void setMemory( void * _memory, size_t _size );
        void * getMemory( size_t & _size ) const;

	public:
		size_t read( void * _buf, size_t _count ) override;
		void seek( size_t _pos ) override;		
		size_t tell() const override;
        size_t size() const override;

    public:
		bool time( time_t & _time ) const override;

    public:
        void destroy() override;

	protected:
		unsigned char* m_data;
		unsigned char* m_pos;
		unsigned char* m_end;

		size_t m_size;
	};
}	// namespace Menge


