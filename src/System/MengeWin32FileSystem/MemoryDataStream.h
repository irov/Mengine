#	pragma once

#	include "DataStream.h"

namespace Menge
{
	class MemoryDataStream 
		: public DataStream
	{
	public:
		MemoryDataStream( void * _pMem, std::streamsize _size, bool _freeOnClose = false );
		~MemoryDataStream();

	public:
		void release() override;
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		void skip( std::streamoff _count ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		void close() override;
		void setFreeOnClose( bool _free ) override;
		void* getBuffer() override;

	protected:
		unsigned char* m_data;
		unsigned char* m_pos;
		unsigned char* m_end;
		bool m_freeOnClose;			
	};
}
