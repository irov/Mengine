#pragma once

#include "Interface/ServiceInterface.h"

#include "Core/ConstString.h"
#include "Core/ConstStringHolder.h"
#include "Core/FilePath.h"
#include "Core/Hash.h"

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Factory/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class StringizeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "StringizeService" )

    public:
        virtual void stringize( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) = 0;

    public:
        virtual void stringizeInternal( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) = 0;
        virtual bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define STRINGIZE_SERVICE()\
    ((Mengine::StringizeServiceInterface*)SERVICE_GET(Mengine::StringizeServiceInterface))
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSizeHash( const Char * _value, ConstStringHolder::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringize( _value, _size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSize( const Char * _value, size_t _size )
        {
            ConstString cstr = stringizeStringSizeHash( _value, (ConstStringHolder::size_type)_size, ((ConstString::hash_type)(-1)) );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeString( const Char * _value )
        {
            ConstString cstr = stringizeStringSize( _value, ((ConstString::size_type)(-1)) );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeString( const String & _value )
        {
            const Char * value_str = _value.c_str();
            String::size_type value_size = _value.size();

            ConstString cstr = stringizeStringSize( value_str, value_size );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeString( const WString & _value )
        {
            String buffer_value( _value.begin(), _value.end() );
            ConstString cstr = stringizeString( buffer_value );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeString( const U32String & _value )
        {
            String buffer_value( _value.begin(), _value.end() );
            ConstString cstr = stringizeString( buffer_value );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, ((ConstString::hash_type)(-1)), cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringHashLocal( const Char * _value, size_t _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, _hash, cstr );

            return cstr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
#	define STRINGIZE_STRING_LOCAL( str )\
	([]{\
        constexpr size_t value_size = (sizeof(str) - 1);\
        constexpr Mengine::ConstString::hash_type value_hash = Mengine::Helper::makeHash( str, value_size);\
        static Mengine::ConstString cstr = Mengine::Helper::stringizeStringHashLocal( str, value_size, value_hash );\
        return cstr;\
    }())
}


