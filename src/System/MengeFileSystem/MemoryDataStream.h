#	pragma once

#	include "DataStream.h"

namespace Menge
{
	class MemoryDataStream 
		: public DataStream
	{
	public:
		MemoryDataStream( void * _pMem, std::size_t _size, bool _freeOnClose = false );
		MemoryDataStream( const String & _name, void* _pMem, std::size_t _size, bool _freeOnClose = false);
		MemoryDataStream( DataStream & _sourceStream, bool _freeOnClose = true);
		MemoryDataStream( DataStream * _sourceStream, bool _freeOnClose = true );
		MemoryDataStream( const String& _name, DataStream& _sourceStream, bool _freeOnClose = true );
		MemoryDataStream( const String& _name, DataStream* _sourceStream, bool _freeOnClose = true);
		MemoryDataStream( std::size_t _size, bool _freeOnClose = true );
		MemoryDataStream( const String& _name, std::size_t _size, bool _freeOnClose = true);

		~MemoryDataStream();

	public:
		unsigned char* getPtr();
		unsigned char* getCurrentPtr();

	public:
		std::size_t read( void* _buf, std::size_t _count ) override;
		std::size_t readLine( char* _buf, std::size_t _maxCount, const String& _delim = "\n" ) override;
		std::size_t skipLine( const String& _delim = "\n" ) override;
		void skip( long _count ) override;
		void seek( std::size_t _pos ) override;
		std::size_t tell() const override;
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