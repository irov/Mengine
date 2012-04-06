#	include "File.h"
#	include "String.h"

#	include "Interface/StreamInterface.h"

namespace Menge
{
	namespace Utils
	{
		//////////////////////////////////////////////////////////////////////////
		const std::streamsize stream_temp_size = 128;
		//////////////////////////////////////////////////////////////////////////
		void skip( InputStreamInterface * _stream, int _count )
		{
			_stream->seek( _stream->tell() + _count );
		}
		//////////////////////////////////////////////////////////////////////////
		bool eof( InputStreamInterface * _stream )
		{
			return _stream->tell() == _stream->size();
		}
		////////////////////////////////////////////////////////////////////////////
		//String getLine( InputStreamInterface* _stream, bool _trimAfter /*= true */ )
		//{
		//	char tmpBuf[stream_temp_size];
		//	String retString;
		//	size_t readCount;
		//	// Keep looping while not hitting delimiter
		//	while ( ( readCount = _stream->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
		//	{
		//		size_t term = readCount;
		//		// Terminate string
		//		tmpBuf[term] = '\0';

		//		char* p = StdStrchr( tmpBuf, '\n' );
		//		if ( p != 0 )
		//		{
		//			// Reposition backwards
		//			skip( _stream, (int)( ( p + 1 - tmpBuf ) - readCount ) );
		//			*p = MENGE_TEXT('\0');
		//		}

		//		retString += tmpBuf;

		//		if ( p != 0 )
		//		{
		//			// Trim off trailing CR if this was a CR/LF entry
		//			if ( retString.length() && retString[retString.length()-1] == '\r' )
		//			{
		//				retString.erase( retString.length() - 1, 1 );
		//			}

		//			// Found terminator, break out
		//			break;
		//		}
		//	}

		//	if( _trimAfter )
		//	{
		//		trim( retString );
		//	}

		//	return retString;
		//}
		//////////////////////////////////////////////////////////////////////////
		//int skipLine( InputStreamInterface* _stream, const String& _delim /*= "\n" */ )
		//{
		//	char tmpBuf[stream_temp_size];
		//	int total = 0;
		//	int readCount;
		//	// Keep looping while not hitting delimiter
		//	while ( ( readCount = _stream->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
		//	{
		//		// Terminate string
		//		int term = readCount / sizeof(TChar);
		//		tmpBuf[term] = '\0';

		//		// Find first delimiter

		//		int pos = strcspn( tmpBuf, _delim.c_str() );

		//		if( pos < term )
		//		{
		//			// Found terminator, reposition backwards
		//			int rep = ( pos + 1 - term );
		//			skip( _stream, rep );

		//			total += ( pos + 1 );

		//			// break out
		//			break;
		//		}

		//		total += readCount;
		//	}

		//	return total;
		//}
		//////////////////////////////////////////////////////////////////////////
		bool getFileExt( const WString & _filename, WString & _out )
		{
			//WString::size_type seppos = _filename.find_last_of( MENGE_FOLDER_DELIM );

			WString::size_type pos = _filename.find_last_of( L'.' );

			if( pos == WString::npos )
			{
				return false;
			}

			_out = _filename.substr( pos + 1, _filename.length() );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void stringWrite( OutputStreamInterface* _stream, const String& _string )
		{
			_stream->write( (void *)_string.c_str(), _string.size() * sizeof(String::value_type) );
		}
		//////////////////////////////////////////////////////////////////////////
		void stringWriteU( OutputStreamInterface * _stream, const WString& _string )
		{
			_stream->write( (void *)_string.c_str(), _string.size() * sizeof(WString::value_type) );
		}
	}
}