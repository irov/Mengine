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
		std::size_t size() const;


		template<typename T> DataStream& operator>>( T& _val );

		virtual std::size_t readLine( char* _buf, std::size_t _maxCount, const String& _delim = MENGE_TEXT("\n") );
		virtual String getLine( bool _trimAfter = true );
		virtual String getAsString( void );
		virtual std::size_t skipLine( const String& _delim = MENGE_TEXT("\n") );

		virtual std::size_t read( void* _buf, std::size_t _count ) = 0;
		virtual void skip( long _count ) = 0;
		virtual void seek( std::size_t _pos ) = 0;
		virtual std::size_t tell() const = 0;
		virtual bool eof() const = 0;

		virtual void close( void ) = 0;
		virtual void* getBuffer() = 0;
		virtual bool isMemory() const = 0;
		virtual void setFreeOnClose( bool _free ) = 0;

	protected:
		String m_name;		
		std::size_t m_size;
	};
}