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
		FileStreamDataStream( const String& _name, std::ifstream* _s, std::size_t _size, bool _freeOnClose = true );

		~FileStreamDataStream();

	public:
		std::size_t read( void* _buf, std::size_t _count ) override;
		std::size_t readLine( char* _buf, std::size_t _maxCount, const String& _delim = "\n") override;

		void skip( long _count ) override;
		void seek( std::size_t _pos ) override;
		std::size_t tell() const override;
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