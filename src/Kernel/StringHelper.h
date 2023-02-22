#pragma once

#include "Kernel/String.h"
#include "Kernel/VectorString.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        String stringFormat( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 1, 2 );
        //////////////////////////////////////////////////////////////////////////
        String stringInt32( int32_t _value );
        String stringFloat( float _value );
        //////////////////////////////////////////////////////////////////////////
        void split( VectorString * const _outStrings, const String & _str, bool _trimDelims, const String & _delim );
        void wsplit( VectorWString * const _outStrings, const WString & _str, bool _trimDelims, const WString & _delim );
        //////////////////////////////////////////////////////////////////////////
        void split2( VectorString * const _outStrings, const String & _str, bool _trimDelims, const VectorString & _delims );
        void u32split2( VectorU32String * const _outStrings, const U32String & _str, bool _trimDelims, const VectorU32String & _delims );
        //////////////////////////////////////////////////////////////////////////
        bool u32split3( VectorU32String * const _outStrings, const U32String & _str, const VectorU32String & _delims );
        //////////////////////////////////////////////////////////////////////////
        void join( const String & _delim, const VectorString & _stringArray, String * const _outString );
        void trim( String * const _str, bool _left = true, bool _right = true );
        //////////////////////////////////////////////////////////////////////////
        bool toupper( const Char * _str, Char * _upper, size_t _capacity );
        void toupper( const String & _in, String * const _upper );
        //////////////////////////////////////////////////////////////////////////
        bool tolower( const Char * _str, Char * _upper, size_t _capacity );
        void tolower( const String & _str, String * const _lower );
        //////////////////////////////////////////////////////////////////////////
    }
};
