#	pragma once

#	include "DataStream.h"

namespace Menge
{
	class FileHandleDataStream 
		: public DataStream
	{
	public:
		FileHandleDataStream( FILE* _handle );
		FileHandleDataStream( const String& _name, FILE* _handle);
		~FileHandleDataStream();

	public:
		std::size_t read( void* _buf, std::size_t _count ) override;
		void skip( long _count ) override;
		void seek( std::size_t _pos ) override;
		std::size_t tell() const override;
		bool eof() const override;
		void close() override;
		void* getBuffer() override;
		bool isMemory() const override;
		void setFreeOnClose( bool _free ) override;

	protected:
		FILE* m_fileHandle;
	};
}