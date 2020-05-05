#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/HashHelper.h"
#include "Kernel/String.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeString( const Char * _value );
        ConstString stringizeString( const String & _value );
        ConstString stringizeStringSize( const Char * _value, ConstStringHolder::size_type _size );
        ConstString stringizeStringSizeHash( const Char * _value, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        ConstString stringizeStringFormat( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 1, 2 );
        ConstString stringizeStringLocal( const Char * _value, ConstStringHolder::size_type _size );
        ConstString stringizeStringHashLocal( const Char * _value, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        ConstString stringizeStringHashUnique( const Char * _value, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        //////////////////////////////////////////////////////////////////////////
        template<ConstString::hash_type Hash>
        const ConstString & stringizeStringTemplate( const Char * _value, ConstStringHolder::size_type _size )
        {
            static ConstString cstr = Helper::stringizeStringHashUnique( _value, _size, Hash );

            return cstr;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_STRING_LOCAL( STRING )\
    Mengine::Helper::stringizeStringTemplate<STRING##_hash>( STRING, (sizeof(STRING) - 1) )
//////////////////////////////////////////////////////////////////////////