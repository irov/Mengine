#	pragma once

#	include "DataStream.h"

namespace Menge
{
	class FileHandleDataStream 
		: public DataStream
	{
	public:
		FileHandleDataStream( FILE* _handle );
		~FileHandleDataStream();

	public:
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		void skip( std::streamoff _count ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		void close() override;
		void* getBuffer() override;
		void setFreeOnClose( bool _free ) override;

	protected:
		FILE* m_fileHandle;
	};
}
