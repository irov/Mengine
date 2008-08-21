#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class DataStream 
		: public DataStreamInterface
	{
	public:
		DataStream();
		DataStream( const String& _name );
		virtual ~DataStream() {}

		const String& getName() const;
		std::streamsize size() const;


		template<typename T> DataStream& operator>>( T& _val );

		virtual std::streamsize readLine( char* _buf, std::streamsize _maxCount, const String& _delim = MENGE_TEXT("\n") );
		virtual String getLine( bool _trimAfter = true );
		virtual String getAsString( void );
		virtual std::streamsize skipLine( const String& _delim = MENGE_TEXT("\n") );

		virtual std::streamsize read( void* _buf, std::streamsize _count ) = 0;
		virtual void skip( std::streampos _count ) = 0;
		virtual void seek( std::streamoff _pos ) = 0;
		virtual std::streampos tell() const = 0;
		virtual bool eof() const = 0;

		virtual void close( void ) = 0;
		virtual void* getBuffer() = 0;
		virtual bool isMemory() const = 0;
		virtual void setFreeOnClose( bool _free ) = 0;

	protected:
		String m_name;		
		std::streamsize m_size;
	};
}