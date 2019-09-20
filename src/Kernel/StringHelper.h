#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/VectorString.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    namespace Helper
    {
        String stringFormat( const Char * _format, ... );

        void split( VectorString & _outStrings, const String & _str, bool _trimDelims, const String & _delim );
        void wsplit( VectorWString & _outStrings, const WString & _str, bool _trimDelims, const WString & _delim );

        void split2( VectorString & _outStrings, const String & _str, bool _trimDelims, const VectorString & _delims );
        void u32split2( VectorU32String & _outStrings, const U32String & _str, bool _trimDelims, const VectorU32String & _delims );

        bool u32split3( VectorU32String & _outStrings, const U32String & _str, const VectorU32String & _delims );

        void join( const String & _delim, const VectorString & _stringArray, String & _outString );
        void trim( String & str, bool left = true, bool right = true );

        bool intToString( int32_t _value, String & _str );
        bool unsignedToString( uint32_t _value, String & _str );
        bool unsigned64ToString( uint64_t _value, String & _str );
        bool floatToString( float _value, String & _str );

        bool intToWString( int32_t _value, WString & _str );
        bool unsignedToWString( uint32_t _value, WString & _str );
        bool unsigned64ToWString( uint64_t _value, WString & _str );
        bool floatToWString( float _value, WString & _str );

        bool stringToInt( const String & _str, int32_t & _value );
        bool stringToUnsigned( const String & _str, uint32_t & _value );
        bool stringToFloat( const String & _str, float & _value );

        bool wstringToInt( const WString & _str, int32_t & _value );
        bool wstringToInt2( const WString & _str, int32_t & _value1, int32_t & _value2, bool _separator );
        bool wstringToInt4( const WString & _str, int32_t & _value1, int32_t & _value2, int32_t & _value3, int32_t & _value4, bool _separator );
        bool wstringToUnsigned( const WString & _str, uint32_t & _value );
        bool wstringToUnsigned64( const WString & _str, uint64_t & _value );
        bool wstringToFloat( const WString & _str, float & _value );
    }
};
