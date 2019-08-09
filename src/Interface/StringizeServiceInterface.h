#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"

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
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_SERVICE()\
    ((Mengine::StringizeServiceInterface*)SERVICE_GET(Mengine::StringizeServiceInterface))
//////////////////////////////////////////////////////////////////////////


