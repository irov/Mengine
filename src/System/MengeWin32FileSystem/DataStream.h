#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class DataStream 
		: public DataStreamInterface
	{
	public:
		DataStream();
		virtual ~DataStream() {}

		std::streamsize size() const;


		template<typename T> DataStream& operator>>( T& _val );
		virtual std::streamsize read( void* _buf, std::streamsize _count ) = 0;
		virtual void seek( std::streamoff _pos ) = 0;
		virtual std::streampos tell() const = 0;
		virtual bool eof() const = 0;

		virtual void close( void ) = 0;

	protected:
		std::streamsize m_size;
	};
}
