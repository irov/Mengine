#pragma once

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Factory.h"

#include "ConstStringHolderMemory.h"

#include "stdex/intrusive_list.h"
#include "stdex/template_pool.h"

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
        bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) override;

    protected:
        typedef stdex::template_pool<ConstStringHolderMemory, 1024> PoolConstStringHolderMemory;
        PoolConstStringHolderMemory m_poolHolderStringMemory;

        typedef stdex::intrusive_list<ConstStringHolder> IntrusiveListConstStringHolder;
        IntrusiveListConstStringHolder m_holdres[4096];

        struct InternalHolder
        {
            const Char * str;
            const ConstStringHolder * holder;
        };

        InternalHolder m_internals[257][8];

    protected:
        ConstStringHolder * testHolder_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash );
        ConstStringHolder * stringizeHolder_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash );
        void addHolder_( ConstStringHolder * _holder, ConstString::hash_type _hash );
        IntrusiveListConstStringHolder & getList_( ConstStringHolder::hash_type _hash );
    };
}
