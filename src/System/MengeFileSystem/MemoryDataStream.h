#	pragma once

#	include "DataStream.h"

namespace Menge
{
	class MemoryDataStream 
		: public DataStream
	{
	public:
		MemoryDataStream( void * _pMem, std::streamsize _size, bool _freeOnClose = false );
		MemoryDataStream( const String & _name, void* _pMem, std::streamsize _size, bool _freeOnClose = false);
		MemoryDataStream( DataStream & _sourceStream, bool _freeOnClose = true);
		MemoryDataStream( DataStream * _sourceStream, bool _freeOnClose = true );
		MemoryDataStream( const String& _name, DataStream& _sourceStream, bool _freeOnClose = true );
		MemoryDataStream( const String& _name, DataStream* _sourceStream, bool _freeOnClose = true);
		MemoryDataStream( std::streamsize _size, bool _freeOnClose = true );
		MemoryDataStream( const String& _name, std::streamsize _size, bool _freeOnClose = true);

		~MemoryDataStream();

	public:
		unsigned char* getPtr();
		unsigned char* getCurrentPtr();

	public:
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		std::streamsize skipLine( const String& _delim = MENGE_TEXT("\n") ) override;
		void skip( std::streampos _count ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		void close() override;
		void setFreeOnClose( bool _free ) override;
		void* getBuffer() override;

		bool isMemory() const override;

	protected:
		unsigned char* m_data;
		unsigned char* m_pos;
		unsigned char* m_end;
		bool m_freeOnClose;			
	};
}