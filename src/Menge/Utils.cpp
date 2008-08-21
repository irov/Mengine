
#	include "Config/Typedef.h"
#	include "Application.h"
#	include "Holder.h"
#	include <sstream>

namespace Menge
{
	namespace Utils
	{
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
		String toString( int x )
		{
			Stringstream str;
			str << x;
			return str.str();
		}
		//////////////////////////////////////////////////////////////////////////
		StringA WToA( const StringW& _stringw )
		{
			return Holder<Application>::hostage()->WToA( _stringw );
		}
		//////////////////////////////////////////////////////////////////////////
		Menge::StringW AToW( const StringA& _stringa )
		{
			return Holder<Application>::hostage()->AToW( _stringa );
		}
		//////////////////////////////////////////////////////////////////////////
	}
}