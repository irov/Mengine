#	include "DataStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	const std::streamsize stream_temp_size = 128;
	//////////////////////////////////////////////////////////////////////////
	DataStream::DataStream()
		: m_size( 0 ) 
	{
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
		TChar tmpBuf[stream_temp_size];
		String retString;
		std::size_t readCount;
		// Keep looping while not hitting delimiter
		while ( ( readCount = read( tmpBuf, ( stream_temp_size - 1 ) * sizeof(TChar) ) ) != 0 )
		{
			std::size_t term = readCount / sizeof(TChar);
			// Terminate string
			tmpBuf[term] = MENGE_TEXT('\0');

			TChar* p = StdStrchr( tmpBuf, MENGE_TEXT('\n') );
			if ( p != 0 )
			{
				// Reposition backwards
				skip( (long)( ( p + 1 - tmpBuf ) * sizeof(TChar) - readCount ) );
				*p = MENGE_TEXT('\0');
			}

			retString += tmpBuf;

			if ( p != 0 )
			{
				// Trim off trailing CR if this was a CR/LF entry
				if ( retString.length() && retString[retString.length()-1] == MENGE_TEXT('\r') )
				{
					retString.erase( retString.length() - 1, 1 );
				}

				// Found terminator, break out
				break;
			}
		}

		if ( _trimAfter )
		{
			static const String delims = MENGE_TEXT(" \t\r");
			retString.erase( retString.find_last_not_of( delims ) + 1 ); // trim right
			retString.erase( 0, retString.find_first_not_of( delims ) ); // trim left
		}

		return retString;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize DataStream::skipLine( const String& _delim )
	{
		TChar tmpBuf[stream_temp_size];
		std::streamsize total = 0;
		std::streamsize readCount;
		// Keep looping while not hitting delimiter
		while ( ( readCount = read( tmpBuf, ( stream_temp_size - 1 ) * sizeof(TChar) ) ) != 0 )
		{
			// Terminate string
			std::streamsize term = readCount / sizeof(TChar);
			tmpBuf[term] = MENGE_TEXT('\0');

			// Find first delimiter

			std::streamsize pos = (std::streamsize)wcscspn( tmpBuf, _delim.c_str() );

			if( pos < term )
			{
				// Found terminator, reposition backwards
				skip( ( pos + 1 - term ) * sizeof(TChar) );

				total += ( pos + 1 ) * sizeof(TChar);

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
		TChar* pBuf = new TChar[(m_size+1)/sizeof(TChar)];
		// Ensure read from begin of stream
		seek(0);
		read( pBuf, m_size );
		pBuf[m_size/sizeof(TChar)] = MENGE_TEXT('\0');
		String str;
		str.insert( 0, pBuf, m_size/sizeof(TChar) );
		delete [] pBuf;
		return str;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize DataStream::size() const
	{
		return m_size;
	}
}
