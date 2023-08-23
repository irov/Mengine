#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static constexpr ConstStringHolder::size_type MENGINE_STRINGIZE_UNKNOWN_SIZE = MENGINE_UNKNOWN_SIZE;
    static constexpr ConstStringHolder::hash_type MENGINE_STRINGIZE_UNKNOWN_HASH = ~0ULL;
    //////////////////////////////////////////////////////////////////////////
    class StringizeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "StringizeService" )

    public:
        virtual void stringize( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash, ConstString * const _cstr ) = 0;

    public:
        virtual void stringizeInternal( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash, ConstString * const _cstr ) = 0;
        virtual void stringizeUnique( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash, ConstString * const _cstr ) = 0;
        virtual bool stringizeExternal( ConstStringHolder * const _holder, ConstString * const _cstr ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define STRINGIZE_SERVICE()\
    ((Mengine::StringizeServiceInterface*)SERVICE_GET(Mengine::StringizeServiceInterface))
//////////////////////////////////////////////////////////////////////////


