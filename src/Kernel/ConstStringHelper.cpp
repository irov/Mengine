#include "ConstStringHelper.h"

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Assertion.h"

#include "Config/StdArg.h"
#include "Config/StdIO.h"
#include "Config/Algorithm.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeString( const Char * _value )
        {
            ConstString cstr = Helper::stringizeStringSize( _value, MENGINE_STRINGIZE_UNKNOWN_SIZE );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringSize( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr = Helper::stringizeStringSizeHash( _value, _size, MENGINE_STRINGIZE_UNKNOWN_HASH );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringSizeHash( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringize( _value, _size, _hash, &cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringFormat( const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            Char str[2048] = {'\0'};
            int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, 2048, _format, args );

            MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid stringize string format '%s'"
                , _format
            );

            MENGINE_VA_LIST_END( args );

            if( size_vsnprintf == 0 )
            {
                return ConstString::none();
            }

            ConstString constString = Helper::stringizeStringSize( str, size_vsnprintf );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size )
        {
            ConstString constString;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, _size, MENGINE_STRINGIZE_UNKNOWN_HASH, &constString );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringHashLocal( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString constString;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, _size, _hash, &constString );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString stringizeStringHashUnique( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString constString;
            STRINGIZE_SERVICE()
                ->stringizeUnique( _value, _size, _hash, &constString );

            return constString;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringizeExistSymbol( const ConstString & _value, Char _symbol )
        {
            const Char * str = _value.c_str();
            size_t size = _value.size();
            const Char * str_end = str + size;

            if( Algorithm::find( str, str_end, _symbol ) == str_end )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}