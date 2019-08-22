#pragma once

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Pool.h"

#include "ConstStringHolderMemory.h"

#include "stdex/intrusive_list.h"


#ifndef MENGINE_STRINGIZE_HOLDER_COUNT
#define MENGINE_STRINGIZE_HOLDER_COUNT 4096
#endif

#ifndef MENGINE_STRINGIZE_INTERNAL_COUNT
#define MENGINE_STRINGIZE_INTERNAL_COUNT 1021
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class StringizeService
        : public ServiceBase<StringizeServiceInterface>
    {
    public:
        StringizeService();
        ~StringizeService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void stringize( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) override;

    public:
        void stringizeInternal( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) override;
        void stringizeUnique( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) override;
        bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) override;

    protected:
        typedef Pool<ConstStringHolderMemory, 1024> PoolConstStringHolderMemory;
        PoolConstStringHolderMemory m_poolHolderStringMemory;

        typedef stdex::intrusive_list<ConstStringHolder> IntrusiveListConstStringHolder;
        IntrusiveListConstStringHolder m_holders[MENGINE_STRINGIZE_HOLDER_COUNT];

        typedef stdex::intrusive_list<ConstStringHolderMemory> IntrusiveListConstStringHolderMemory;
        IntrusiveListConstStringHolderMemory m_holdersMemory;

        struct InternalHolder
        {
            const ConstStringHolder::value_type * str;
            const ConstStringHolder * holder;
        };

        InternalHolder m_internals[MENGINE_STRINGIZE_INTERNAL_COUNT][8];

    protected:
        ConstStringHolder * testHolder_( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        ConstStringHolder * stringizeHolderUnique_( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        ConstStringHolder * stringizeHolder_( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        void addHolder_( ConstStringHolder * _holder, ConstString::hash_type _hash );
        IntrusiveListConstStringHolder & getList_( ConstStringHolder::hash_type _hash );
    };
}
