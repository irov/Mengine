#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class MemoryDataStream 
		: public DataStreamInterface
	{
	public:
		MemoryDataStream( void * _pMem, std::streamsize _size );
		~MemoryDataStream();

	public:
		void release() override;
		
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		void skip( std::streamoff _count ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		std::streamsize size() const;

	protected:
		unsigned char* m_data;
		unsigned char* m_pos;
		unsigned char* m_end;
		std::streamsize m_size;
	};
}