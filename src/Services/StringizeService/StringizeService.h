#pragma once

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "ConstStringHolderMemory.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/IntrusiveList.h"

#ifndef MENGINE_STRINGIZE_HOLDER_COUNT
#define MENGINE_STRINGIZE_HOLDER_COUNT 4096
#endif

#ifndef MENGINE_STRINGIZE_INTERNAL_COUNT
#define MENGINE_STRINGIZE_INTERNAL_COUNT 1021
#endif

namespace Mengine
{
    class StringizeService
        : public ServiceBase<StringizeServiceInterface>
    {
        DECLARE_FACTORABLE( StringizeService );

    public:
        StringizeService();
        ~StringizeService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void stringize( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash, ConstString * const _cstr ) override;

    public:
        void stringizeInternal( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash, ConstString * const _cstr ) override;
        void stringizeUnique( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash, ConstString * const _cstr ) override;
        bool stringizeExternal( ConstStringHolder * const _holder, ConstString * const _cstr ) override;

    protected:
        typedef Pool<ConstStringHolderMemory, 1024> PoolConstStringHolderMemory;
        PoolConstStringHolderMemory m_poolHolderStringMemory;

        typedef IntrusiveList<ConstStringHolder> IntrusiveListConstStringHolder;
        IntrusiveListConstStringHolder m_holders[MENGINE_STRINGIZE_HOLDER_COUNT];

        typedef IntrusiveList<ConstStringHolderMemory> IntrusiveListConstStringHolderMemory;
        IntrusiveListConstStringHolderMemory m_holdersMemory;

        struct InternalHolder
        {
            const ConstStringHolder::value_type * str = nullptr;
            const ConstStringHolder * holder = nullptr;
        };

        InternalHolder m_internals[MENGINE_STRINGIZE_INTERNAL_COUNT][8];

    protected:
        const ConstStringHolder * testHolder_( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash ) const;
        const ConstStringHolder * stringizeHolderUnique_( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        const ConstStringHolder * stringizeHolder_( const ConstStringHolder::value_type * _str, ConstStringHolder::size_type _size, ConstStringHolder::hash_type _hash );
        void addHolder_( ConstStringHolder * _holder, ConstStringHolder::hash_type _hash );
        IntrusiveListConstStringHolder & getList_( ConstStringHolder::hash_type _hash );
        const IntrusiveListConstStringHolder & getList_( ConstStringHolder::hash_type _hash ) const;
    };
}
