#	pragma once

#	include "DataStream.h"

#	include <fstream>

namespace Menge
{
	class FileStreamDataStream 
		: public DataStream
	{
	public:
		FileStreamDataStream( std::ifstream* _s, bool _freeOnClose = true );
		FileStreamDataStream( const String& _name, std::ifstream* _s, bool _freeOnClose = true );
		FileStreamDataStream( const String& _name, std::ifstream* _s, std::streamsize _size, bool _freeOnClose = true );

		~FileStreamDataStream();

	public:
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		std::streamsize readLine( char* _buf, std::streamsize _maxCount, const String& _delim = MENGE_TEXT("\n") ) override;

		void skip( std::streampos _count ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		void close() override;
		void* getBuffer() override;
		bool isMemory() const override;
		void setFreeOnClose( bool _free ) override;

	protected:
		/// Reference to source stream
		std::ifstream* m_stream;
		bool m_freeOnClose;
		bool m_freeBuffer;
		unsigned char* m_buffer;
	};
}