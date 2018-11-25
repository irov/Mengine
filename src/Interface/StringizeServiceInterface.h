#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"
#include "Kernel/FilePath.h"
#include "Kernel/HashHelper.h"

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Kernel/Factorable.h"

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
        virtual void stringizeUnique( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) = 0;        
        virtual bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define STRINGIZE_SERVICE()\
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
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringHashUnique( const Char * _value, size_t _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeUnique( _value, (ConstString::size_type)_size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        template<ConstString::hash_type Hash>
        inline ConstString stringizeStringTemplate( const Char * _value, ConstString::size_type _size )
        {
            static ConstString cstr = Helper::stringizeStringHashUnique( _value, _size, Hash );

            return cstr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    inline ConstString operator "" _c( const Char * _value, size_t _size )
    {
        ConstString cstr = Helper::stringizeStringSize( _value, (ConstString::size_type)_size );

        return cstr;
    }
    //////////////////////////////////////////////////////////////////////////
    constexpr ConstString::hash_type operator "" _hash( const Char * _value, size_t _size )
    {
        return Mengine::Helper::makeHash( _value, _size );
    }
    //////////////////////////////////////////////////////////////////////////
#	define STRINGIZE_STRING_LOCAL( str )\
        Mengine::Helper::stringizeStringTemplate<str##_hash>( str, (sizeof(str) - 1) )
}


