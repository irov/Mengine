#include "ConstStringHelper.h"

#include "Interface/StringizeServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringSizeHash( const Char * _value, ConstStringHolder::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringize( _value, _size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringSize( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr = stringizeStringSizeHash( _value, (ConstStringHolder::size_type)_size, MENGINE_STRINGIZE_UNKNOWN_HASH );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeString( const Char * _value )
        {
            ConstString cstr = stringizeStringSize( _value, MENGINE_STRINGIZE_UNKNOWN_SIZE );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeString( const String & _value )
        {
            const Char * value_str = _value.c_str();
            String::size_type value_size = _value.size();

            ConstString cstr = stringizeStringSize( value_str, static_cast<ConstString::size_type>(value_size) );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, MENGINE_STRINGIZE_UNKNOWN_HASH, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringHashLocal( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringHashUnique( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeUnique( _value, (ConstString::size_type)_size, _hash, cstr );

            return cstr;
        }        
    }
}