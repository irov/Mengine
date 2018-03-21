#pragma once

#include "Interface/ServiceInterface.h"

#include "Core/ConstString.h"
#include "Core/ConstStringHolder.h"
#include "Core/FilePath.h"

#include "Config/Typedef.h"
#include "Config/String.h"

#   include "Factory/FactorablePtr.h"

namespace Mengine
{
    class StringizeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "StringizeService" )

    public:
        virtual void stringize( const Char * _str, ConstString::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) = 0;

    public:
        virtual void stringizeInternal( const Char * _str, ConstString::size_type _size, ConstString & _cstr ) = 0;
        virtual bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) = 0;
    };

#   define STRINGIZE_SERVICE()\
    ((Mengine::StringizeServiceInterface*)SERVICE_GET(Mengine::StringizeServiceInterface))

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSizeHash( const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringize( _value, _size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSize( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr = stringizeStringSizeHash( _value, _size, ((ConstString::hash_type)(-1)) );

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
            ConstString cstr = stringizeStringSize( _value.c_str(), _value.size() );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringLocal( const Char * _value, ConstString::size_type _size )
        {
            ConstString cstr;
            STRINGIZE_SERVICE()
                ->stringizeInternal( _value, _size, cstr );

            return cstr;
        }
    }

#	define STRINGIZE_STRING_LOCAL( str )\
	Mengine::Helper::stringizeStringLocal( str, (sizeof(str) - 1) )
}


