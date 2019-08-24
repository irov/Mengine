#include "ConstStringHelper.h"

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Assertion.h"

#include "Config/StdIO.h"

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
        ConstString stringizeStringFormat( const Char * _format, ... )
        {
            va_list args;
            va_start( args, _format );

            Char str[2048] = { 0 };
            int size = MENGINE_VSNPRINTF( str, 2047, _format, args );

            MENGINE_ASSERTION_FATAL( size >= 0, "invalid stringize string format '%s'"
                , _format
            );

            va_end( args );

            if( size == 0 )
            {
                return ConstString::none();
            }

            ConstString constString = Helper::stringizeStringSize( str, size );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeString( const String & _value )
        {
            const String::value_type * value_str = _value.c_str();
            String::size_type value_size = _value.size();

            ConstString constString = stringizeStringSize( value_str, static_cast<ConstString::size_type>(value_size) );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size )
        {
            ConstString constString;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, MENGINE_STRINGIZE_UNKNOWN_HASH, constString );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringHashLocal( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString constString;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, _hash, constString );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringHashUnique( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString constString;
            STRINGIZE_SERVICE()
                ->stringizeUnique( _value, (ConstString::size_type)_size, _hash, constString );

            return constString;
        }
    }
}