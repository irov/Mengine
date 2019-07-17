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
    static const ConstStringHolder::size_type MENGINE_STRINGIZE_UNKNOWN_SIZE = ~0U;
    static const ConstString::hash_type MENGINE_STRINGIZE_UNKNOWN_HASH = -1;
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
        MENGINE_INLINE ConstString stringizeStringSizeHash( const Char * _value, ConstStringHolder::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringize( _value, _size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE ConstString stringizeStringSize( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr = stringizeStringSizeHash( _value, (ConstStringHolder::size_type)_size, MENGINE_STRINGIZE_UNKNOWN_HASH );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE ConstString stringizeString( const Char * _value )
        {
            ConstString cstr = stringizeStringSize( _value, MENGINE_STRINGIZE_UNKNOWN_SIZE );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE ConstString stringizeString( const String & _value )
        {
            const Char * value_str = _value.c_str();
            String::size_type value_size = _value.size();

            ConstString cstr = stringizeStringSize( value_str, static_cast<ConstString::size_type>(value_size) );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, MENGINE_STRINGIZE_UNKNOWN_HASH, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE ConstString stringizeStringHashLocal( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, (ConstString::size_type)_size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE ConstString stringizeStringHashUnique( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeUnique( _value, (ConstString::size_type)_size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        template<ConstString::hash_type Hash>
        MENGINE_INLINE ConstString stringizeStringTemplate( const Char * _value, ConstString::size_type _size )
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


