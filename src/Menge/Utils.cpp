
#	include "Config/Typedef.h"
#	include "Utils.h"
#	include <sstream>
#	include "Interface/FileSystemInterface.h"
#	include <clocale>

namespace Menge
{
	namespace Utils
	{
		//////////////////////////////////////////////////////////////////////////
		const std::streamsize stream_temp_size = 128;
		//////////////////////////////////////////////////////////////////////////
		const String& emptyString()
		{
			static String empty;
			return empty;
		}
		//////////////////////////////////////////////////////////////////////////
		TStringVector split( const String& _str, const String& _delims /*= "\t\n "*/, unsigned int _maxSplits /*= 0 */ )
		{
			TStringVector ret;
			// Pre-allocate some space for performance
			ret.reserve(_maxSplits ? _maxSplits+1 : 10);    // 10 is guessed capacity for most case

			unsigned int numSplits = 0;

			// Use STL methods 
			size_t start, pos;
			start = 0;
			do 
			{
				pos = _str.find_first_of(_delims, start);
				if (pos == start)
				{
					// Do nothing
					start = pos + 1;
				}
				else if (pos == String::npos || (_maxSplits && numSplits == _maxSplits))
				{
					// Copy the rest of the string
					ret.push_back( _str.substr(start) );
					break;
				}
				else
				{
					// Copy up to delimiter
					ret.push_back( _str.substr(start, pos - start) );
					start = pos + 1;
				}
				// parse up to next real data
				start = _str.find_first_not_of(_delims, start);
				++numSplits;

			} while (pos != String::npos);

			return ret;
		}
		//////////////////////////////////////////////////////////////////////////
		void trim( String& str, bool left/* = true*/, bool right/* = true */)
		{
			static const String delims = " \t\r";
			if(right)
			{
				str.erase(str.find_last_not_of(delims)+1); // trim right
			}
			if(left)
			{
				str.erase(0, str.find_first_not_of(delims)); // trim left
			}
		}
		//////////////////////////////////////////////////////////////////////////
		String toString( int x )
		{
			Stringstream str;
			str << x;
			return str.str();
		}
		//////////////////////////////////////////////////////////////////////////
		String toString( std::size_t _x )
		{
			Stringstream str;
			str << _x;
			return str.str();
		}
		//////////////////////////////////////////////////////////////////////////
		StringA WToA( const StringW& _value )
		{
			//return Holder<Application>::hostage()->WToA( _stringw );
			std::size_t converted = 0;
			std::size_t size = _value.size() + 1;
			TCharA* stra = new TCharA[size];
			//wcstombs_s( &converted, stra, size, _stringw.c_str(), _TRUNCATE );
			wcstombs( stra, _value.c_str(), size );
			StringA out( stra );
			delete[] stra;
			return out;
		}
		//////////////////////////////////////////////////////////////////////////
		StringW AToW( const StringA& _value )
		{
			setlocale( LC_CTYPE, "" );
			//return Holder<Application>::hostage()->AToW( _String );
			std::size_t converted = 0;
			std::size_t size = _value.size() + 1;
			TCharW* strw = new TCharW[size];
			//mbstowcs_s( &converted, strw, size, _String.c_str(), _TRUNCATE );
			mbstowcs( strw, _value.c_str(), size );
			StringW out( strw );
			delete[] strw;
			return out;
		}
		//////////////////////////////////////////////////////////////////////////
		String getLine( DataStreamInterface* _stream, bool _trimAfter /*= true */ )
		{
			TChar tmpBuf[stream_temp_size];
			String retString;
			std::size_t readCount;
			// Keep looping while not hitting delimiter
			while ( ( readCount = _stream->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
			{
				std::size_t term = readCount;
				// Terminate string
				tmpBuf[term] = '\0';

				TChar* p = StdStrchr( tmpBuf, '\n' );
				if ( p != 0 )
				{
					// Reposition backwards
					_stream->skip( (long)( ( p + 1 - tmpBuf ) - readCount ) );
					*p = MENGE_TEXT('\0');
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

			if( _trimAfter )
			{
				trim( retString );
			}

			return retString;
		}
		//////////////////////////////////////////////////////////////////////////
		std::streamsize skipLine( DataStreamInterface* _stream, const String& _delim /*= "\n" */ )
		{
			TChar tmpBuf[stream_temp_size];
			std::streamsize total = 0;
			std::streamsize readCount;
			// Keep looping while not hitting delimiter
			while ( ( readCount = _stream->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
			{
				// Terminate string
				int term = readCount / sizeof(TChar);
				tmpBuf[term] = '\0';

				// Find first delimiter

				int pos = strcspn( tmpBuf, _delim.c_str() );

				if( pos < term )
				{
					// Found terminator, reposition backwards
					std::streamoff rep = ( pos + 1 - term );
					_stream->skip( rep );

					total += ( pos + 1 );

					// break out
					break;
				}

				total += readCount;
			}

			return total;
		}
		//////////////////////////////////////////////////////////////////////////
		bool getFileExt( String & _out, const String & _filename )
		{
			std::size_t pos = _filename.find_last_of( "." );

			if( pos == String::npos )
			{
				return false;
			}

			_out = _filename.substr( pos + 1, _filename.length() );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
	}
}
