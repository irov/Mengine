#	include "String.h"

#	include "Config/Typedef.h"

#	include <sstream>
#	include <clocale>
#	include <cassert>

#	include <stdlib.h>

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
		void split( TVectorString & _outStrings, const String& _str, bool _trimDelims, const String& _delims /*= "\t\n "*/, unsigned int _maxSplits /*= 0 */ )
		{
			// Pre-allocate some space for performance
			_outStrings.reserve(_maxSplits ? _maxSplits+1 : 10);    // 10 is guessed capacity for most case

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
					_outStrings.push_back( _str.substr(start) );
					break;
				}
				else
				{
					// Copy up to delimiter
					_outStrings.push_back( _str.substr(start, pos - start) );
					start = pos + 1;
				}
				// parse up to next real data
				if( _trimDelims == true )
				{
					start = _str.find_first_not_of(_delims, start);
				}
				++numSplits;

			} while (pos != String::npos);
		}
		//////////////////////////////////////////////////////////////////////////
		void join( const String& _delim, const TVectorString& _stringArray, String & _outString )
		{
			if( _stringArray.empty() == true )
			{
				_outString.clear();
				return;
			}
			TVectorString::const_iterator it = _stringArray.begin();
			_outString = *it;
			++it;
			for( TVectorString::const_iterator it_end = _stringArray.end();
				it != it_end;
				++it )
			{
				_outString += _delim;
				_outString += (*it);
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
	}
}