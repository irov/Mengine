#	pragma once

#	include "DataStream.h"

#	include <fstream>

namespace Menge
{
	class FileStreamDataStream 
		: public DataStream
	{
	public:
		FileStreamDataStream( std::ifstream* _s, std::streamsize _size, bool _freeOnClose = true );
		~FileStreamDataStream();

	public:
		void release() override;

		std::streamsize read( void* _buf, std::streamsize _count ) override;

		void skip( std::streamoff _count ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		void close() override;
		void* getBuffer() override;
		void setFreeOnClose( bool _free ) override;

	protected:
		/// Reference to source stream
		std::ifstream* m_stream;
		bool m_freeOnClose;
		bool m_freeBuffer;
		unsigned char* m_buffer;
	};
}
