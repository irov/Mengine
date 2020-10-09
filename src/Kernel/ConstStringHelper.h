#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/HashHelper.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeString( const Char * _value );
        ConstString stringizeStringSize( const Char * _value, ConstString::size_type _size );
        ConstString stringizeStringSizeHash( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash );
        ConstString stringizeStringFormat( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 1, 2 );
        ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size );
        ConstString stringizeStringHashLocal( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash );
        ConstString stringizeStringHashUnique( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash );
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            template<class T>
            struct StringizeStringHelper
            {
                ConstString operator() ( const T & _value ) const
                {
                    const ConstString::value_type * value_str = _value.c_str();
                    ConstString::size_type value_size = (ConstString::size_type)_value.size();

                    ConstString constString = Helper::stringizeStringSize( value_str, value_size );

                    return constString;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            template<size_t N>
            struct StringizeStringHelper<Char[N]>
            {
                ConstString operator() ( const Char * _value ) const
                {
                    const ConstString::value_type * value_str = _value;

                    ConstString constString = Helper::stringizeString( value_str );

                    return constString;
                }
            };
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        ConstString stringizeString( const T & _value )
        {
            ConstString constString = Detail::StringizeStringHelper<T>()(_value);

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        template<ConstString::hash_type Hash>
        const ConstString & stringizeStringTemplate( const Char * _value, ConstString::size_type _size )
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