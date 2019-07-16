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
        void stringize( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) override;

    public:
        void stringizeInternal( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) override;
        void stringizeUnique( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) override;
        bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) override;

    protected:
        typedef Pool<ConstStringHolderMemory, 1024> PoolConstStringHolderMemory;
        PoolConstStringHolderMemory m_poolHolderStringMemory;

        typedef stdex::intrusive_list<ConstStringHolder> IntrusiveListConstStringHolder;
        IntrusiveListConstStringHolder m_holdres[MENGINE_STRINGIZE_HOLDER_COUNT];

        struct InternalHolder
        {
            const Char * str;
            const ConstStringHolder * holder;
        };

        InternalHolder m_internals[MENGINE_STRINGIZE_INTERNAL_COUNT][8];

    protected:
        ConstStringHolder * testHolder_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash );
        ConstStringHolder * stringizeHolderUnique_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash );
        ConstStringHolder * stringizeHolder_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash );
        void addHolder_( ConstStringHolder * _holder, ConstString::hash_type _hash );
        IntrusiveListConstStringHolder & getList_( ConstStringHolder::hash_type _hash );
    };
}
