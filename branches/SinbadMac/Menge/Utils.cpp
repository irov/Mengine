
#	include "Config/Typedef.h"
#	include "Utils.h"

#	include <sstream>
#	include <clocale>
#	include <cassert>

#	include <stdlib.h>
#	include <string.h>

#	include "FileInterface.h"

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
		TStringVector split( const String& _str, bool _trimDelims, const String& _delims /*= "\t\n "*/, unsigned int _maxSplits /*= 0 */ )
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
				/*if (pos == start)
				{
					// Do nothing
					start = pos + 1;
				}
				else */if (pos == String::npos || (_maxSplits && numSplits == _maxSplits))
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
				if( _trimDelims == true )
				{
					start = _str.find_first_not_of(_delims, start);
				}
				++numSplits;

			} while (pos != String::npos);

			return ret;
		}
		//////////////////////////////////////////////////////////////////////////
		void join( const String& _delim, const TStringVector& _stringArray, String* _outString )
		{
			assert( _outString != NULL );
			if( _stringArray.empty() == true )
			{
				_outString->clear();
				return;
			}
			TStringVector::const_iterator it = _stringArray.begin();
			*_outString = *it;
			++it;
			for( TStringVector::const_iterator it_end = _stringArray.end();
				it != it_end;
				++it )
			{
				(*_outString) += _delim;
				(*_outString) += (*it);
			}
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
		void skip( FileInputInterface* _file, int _count )
		{
			_file->seek( _file->tell() + _count );
		}
		//////////////////////////////////////////////////////////////////////////
		bool eof( FileInputInterface* _file )
		{
			return _file->tell() == _file->size();
		}
		//////////////////////////////////////////////////////////////////////////
		String getLine( FileInputInterface* _file, bool _trimAfter /*= true */ )
		{
			TChar tmpBuf[stream_temp_size];
			String retString;
			std::size_t readCount;
			// Keep looping while not hitting delimiter
			while ( ( readCount = _file->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
			{
				std::size_t term = readCount;
				// Terminate string
				tmpBuf[term] = '\0';

				TChar* p = StdStrchr( tmpBuf, '\n' );
				if ( p != 0 )
				{
					// Reposition backwards
					skip( _file, (int)( ( p + 1 - tmpBuf ) - readCount ) );
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
		int skipLine( FileInputInterface* _file, const String& _delim /*= "\n" */ )
		{
			TChar tmpBuf[stream_temp_size];
			int total = 0;
			int readCount;
			// Keep looping while not hitting delimiter
			while ( ( readCount = _file->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
			{
				// Terminate string
				int term = readCount / sizeof(TChar);
				tmpBuf[term] = '\0';

				// Find first delimiter

				int pos = strcspn( tmpBuf, _delim.c_str() );

				if( pos < term )
				{
					// Found terminator, reposition backwards
					int rep = ( pos + 1 - term );
					skip( _file, rep );

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
		void fileWrite( FileOutputInterface* _file, const String& _string )
		{
			_file->write( _string.c_str(), _string.size() );
		}
		//////////////////////////////////////////////////////////////////////////
		void collapsePath( const String& _path, String* _collapsedPath )
		{
			assert( _collapsedPath != NULL );
			// Don't ignore empty: we do want to keep trailing slashes.
			TStringVector as = Utils::split( _path, false, "/" );

			// pass to remove duplicate '/' ( .//foo, ..//foo )
			for( TStringVector::size_type i = 0
				; i < as.size()
				; /**/ )
			{
				if( as[i].empty() == true && i > 0 && ( as[i-1] == ".." || as[i-1] == "." ) )
				{
					as.erase( as.begin() + i );
				}
				else
				{
					++i;
				}
			}

			for( TStringVector::size_type i = 0; i < as.size(); ++i )
			{
				if( as[i] == ".." && i != 0 )
				{
					/* If the previous element is also "..", then we have a path beginning
					* with multiple "../"--one .. can't eat another .., since that would
					* cause "../../foo" to become "foo". */
					if( as[i-1] != ".." )
					{
						as.erase( as.begin()+i-1 );
						as.erase( as.begin()+i-1 );
						i -= 2;
					}
				}
				else if( as[i] == "" && i != 0 && i != 1 && i+1 < as.size() )
				{
					/* Remove empty parts that aren't at the beginning or end;
					* "foo//bar/" -> "foo/bar/", but "/foo" -> "/foo" and "foo/"
					* to "foo/". */
					as.erase( as.begin()+i );
					i -= 1;
				}
				else if( as[i] == "." )
				{
					as.erase( as.begin()+i );
					i -= 1;
				}
			}
			join( "/", as, _collapsedPath );

		}
		//////////////////////////////////////////////////////////////////////////
	}
}
