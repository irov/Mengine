#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/HashHelper.h"

#include "Config/String.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeString( const Char * _value );
        ConstString stringizeString( const String & _value );
        ConstString stringizeStringSize( const Char * _value, ConstString::size_type _size );
        ConstString stringizeStringSizeHash( const Char * _value, ConstStringHolder::size_type _size, ConstString::hash_type _hash );
        ConstString stringizeStringFormat( const Char * _format, ... );
        ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size );
        ConstString stringizeStringHashLocal( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash );
        ConstString stringizeStringHashUnique( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash );
        //////////////////////////////////////////////////////////////////////////
        template<ConstString::hash_type Hash>
        ConstString stringizeStringTemplate( const Char * _value, ConstString::size_type _size )
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