#	include "DataStream.h"

#	define MENGE_STREAM_TEMP_SIZE 128

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DataStream::DataStream()
		: m_size( 0 ) 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataStream::DataStream( const String& _name )
		: m_name( _name )
		, m_size( 0 ) 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String& DataStream::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	template <typename T> DataStream& DataStream::operator >>( T& _val )
	{
		read(static_cast<void*>(&_val), sizeof(T));
		return *this;
	}
	//////////////////////////////////////////////////////////////////////////
	String DataStream::getLine( bool _trimAfter )
	{
		char tmpBuf[MENGE_STREAM_TEMP_SIZE];
		String retString;
		std::size_t readCount;
		// Keep looping while not hitting delimiter
		while ( ( readCount = read( tmpBuf, MENGE_STREAM_TEMP_SIZE - 1 ) ) != 0 )
		{
			// Terminate string
			tmpBuf[readCount] = '\0';

			char* p = strchr( tmpBuf, '\n' );
			if ( p != 0 )
			{
				// Reposition backwards
				skip( (long)( p + 1 - tmpBuf - readCount ) );
				*p = '\0';
			}

			retString += tmpBuf;

			if ( p != 0 )
			{
				// Trim off trailing CR if this was a CR/LF entry
				if ( retString.length() && retString[retString.length()-1] == '\r' )
				{
					retString.erase( retString.length() - 1, 1 );
				}

				// Found terminator, break out
				break;
			}
		}

		if ( _trimAfter )
		{
			static const String delims = " \t\r";
			retString.erase( retString.find_last_not_of( delims ) + 1 ); // trim right
			retString.erase( 0, retString.find_first_not_of( delims ) ); // trim left
		}

		return retString;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t DataStream::readLine( char* _buf, std::size_t _maxCount, const String& _delim )
	{
		// Deal with both Unix & Windows LFs
		bool trimCR = false;
		if ( _delim.find_first_of('\n') != String::npos )
		{
			trimCR = true;
		}

		char tmpBuf[MENGE_STREAM_TEMP_SIZE];
		std::size_t chunkSize = (std::min)( _maxCount, (std::size_t)MENGE_STREAM_TEMP_SIZE - 1 );
		std::size_t totalCount = 0;
		std::size_t readCount; 
		while ( chunkSize && ( readCount = read( tmpBuf, chunkSize ) ) )
		{
			// Terminate
			tmpBuf[readCount] = '\0';

			// Find first delimiter
			std::size_t pos = strcspn( tmpBuf, _delim.c_str() );

			if( pos < readCount )
			{
				// Found terminator, reposition backwards
				skip( (long)( pos + 1 - readCount ) );
			}

			// Are we genuinely copying?
			if( _buf )
			{
				memcpy( _buf + totalCount, tmpBuf, pos );
			}
			totalCount += pos;

			if( pos < readCount )
			{
				// Trim off trailing CR if this was a CR/LF entry
				if ( trimCR && totalCount && _buf[totalCount-1] == '\r')
				{
					--totalCount;
				}

				// Found terminator, break out
				break;
			}

			// Adjust chunkSize for next time
			chunkSize = (std::min)( _maxCount - totalCount, (std::size_t)MENGE_STREAM_TEMP_SIZE - 1 );
		}

		// Terminate
		_buf[totalCount] = '\0';

		return totalCount;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t DataStream::skipLine( const String& _delim )
	{
		char tmpBuf[MENGE_STREAM_TEMP_SIZE];
		std::size_t total = 0;
		std::size_t readCount;
		// Keep looping while not hitting delimiter
		while ( ( readCount = read( tmpBuf, MENGE_STREAM_TEMP_SIZE - 1 ) ) != 0 )
		{
			// Terminate string
			tmpBuf[readCount] = '\0';

			// Find first delimiter
			std::size_t pos = strcspn( tmpBuf, _delim.c_str() );

			if( pos < readCount )
			{
				// Found terminator, reposition backwards
				skip( (long)( pos + 1 - readCount ) );

				total += pos + 1;

				// break out
				break;
			}

			total += readCount;
		}

		return total;
	}
	//////////////////////////////////////////////////////////////////////////
	String DataStream::getAsString()
	{
		// Read the entire buffer
		char* pBuf = new char[m_size+1];
		// Ensure read from begin of stream
		seek(0);
		read( pBuf, m_size );
		pBuf[m_size] = '\0';
		String str;
		str.insert( 0, pBuf, m_size );
		delete [] pBuf;
		return str;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t DataStream::size() const
	{
		return m_size;
	}
}