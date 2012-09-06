#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

namespace Menge
{	
	typedef std::vector<ConstString> TVectorConstString;

	namespace Utils
	{
		const String& emptyString();
		const WString& emptyWString();
		
		void split( TVectorString & _outStrings, const String& _str, bool _trimDelims, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		void wsplit( TVectorWString & _outStrings, const WString& _str, bool _trimDelims, const WString& _delims = L"\t\n ", unsigned int _maxSplits = 0 );

		void join( const String& _delim, const TVectorString& _stringArray, String & _outString );
		void trim( String& str, bool left = true, bool right = true );

		bool intToString( int _value, String & _str );
		bool unsignedToString( size_t _value, String & _str );
        bool floatToString( float _value, String & _str );

		bool intToWString( int _value, WString & _str );
		bool unsignedToWString( size_t _value, WString & _str );
        bool floatToWString( float _value, WString & _str );

        bool stringToInt( const String & _str, int & _value );
        bool stringToUnsigned( const String & _str, size_t & _value );
        bool wstringToFloat( const String & _str, float & _value );

        bool wstringToInt( const WString & _str, int & _value );
        bool wstringToInt2( const WString & _str, int & _value1, int & _value2, bool _separator );
        bool wstringToInt4( const WString & _str, int & _value1, int & _value2, int & _value3, int & _value4, bool _separator );
        bool wstringToUnsigned( const WString & _str, size_t & _value );
        bool wstringToFloat( const WString & _str, float & _value );
    }
};
