#include "StringizeService.h"

#include "Kernel/HashType.h"

#include "Kernel/FactoryPool.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StringizeService, Mengine::StringizeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StringizeService::StringizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StringizeService::~StringizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool StringizeService::_initializeService()
    {
        for( uint32_t i = 0; i != MENGINE_STRINGIZE_INTERNAL_COUNT; ++i )
        {
            for( uint32_t j = 0; j != 8; ++j )
            {
                InternalHolder & holder = m_internals[i][j];

                holder.str = nullptr;
                holder.holder = nullptr;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StringizeService::_finalizeService()
    {
        for( uint32_t i = 0; i != MENGINE_STRINGIZE_INTERNAL_COUNT; ++i )
        {
            for( uint32_t j = 0; j != 8; ++j )
            {
                InternalHolder & holder = m_internals[i][j];

                holder.str = nullptr;
                holder.holder = nullptr;
            }
        }

        for( uint32_t i = 0; i != 4096; ++i )
        {
            IntrusiveListConstStringHolder & holdres = m_holdres[i];

            holdres.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void StringizeService::stringize( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr )
    {
        if( _size == MENGINE_STRINGIZE_UNKNOWN_SIZE )
        {
            size_t len = strlen( _str );

            _size = (ConstStringHolder::size_type)len;
        }

        if( _size == 0 )
        {
            _cstr = ConstString::none();

            return;
        }

        if( _hash == MENGINE_STRINGIZE_UNKNOWN_HASH )
        {
            _hash = Helper::makeHash( _str, _size );
        }

        ConstStringHolder * holder = this->stringizeHolder_( _str, _size, _hash );

        _cstr = ConstString( holder );
    }
    //////////////////////////////////////////////////////////////////////////
    void StringizeService::stringizeInternal( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr )
    {
        if( _size == 0 )
        {
            _cstr = ConstString::none();

            return;
        }

        if( _hash == MENGINE_STRINGIZE_UNKNOWN_HASH )
        {
            _hash = Helper::makeHash( _str, _size );
        }

        ptrdiff_t ptr_diff = reinterpret_cast<ptrdiff_t>(_str);

        uint32_t ptr_hash = (ptr_diff / sizeof( ptrdiff_t )) % MENGINE_STRINGIZE_INTERNAL_COUNT;

        for( uint32_t i = 0; i != 8; ++i )
        {
            InternalHolder & inter = m_internals[ptr_hash][i];

            if( inter.str == _str )
            {
                _cstr = ConstString( inter.holder );

                return;
            }

            if( inter.holder != nullptr )
            {
                continue;
            }

            ConstStringHolder * holder = this->stringizeHolder_( _str, _size, _hash );

            inter.str = _str;
            inter.holder = holder;

            _cstr = ConstString( holder );

            return;
        }

        ConstStringHolder * holder = this->stringizeHolder_( _str, _size, _hash );

        _cstr = ConstString( holder );
    }
    //////////////////////////////////////////////////////////////////////////
    void StringizeService::stringizeUnique( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash, ConstString & _cstr )
    {
        if( _size == 0 )
        {
            _cstr = ConstString::none();

            return;
        }

        ptrdiff_t ptr_diff = reinterpret_cast<ptrdiff_t>(_str);

        uint32_t ptr_hash = (ptr_diff / sizeof( ptrdiff_t )) % MENGINE_STRINGIZE_INTERNAL_COUNT;

        for( uint32_t i = 0; i != 8; ++i )
        {
            InternalHolder & inter = m_internals[ptr_hash][i];

            if( inter.str == _str )
            {
                _cstr = ConstString( inter.holder );

                return;
            }

            if( inter.holder != nullptr )
            {
                continue;
            }

            ConstStringHolder * holder = this->stringizeHolderUnique_( _str, _size, _hash );

            inter.str = _str;
            inter.holder = holder;

            _cstr = ConstString( holder );

            return;
        }

        ConstStringHolder * holder = this->stringizeHolderUnique_( _str, _size, _hash );

        _cstr = ConstString( holder );
    }
    //////////////////////////////////////////////////////////////////////////
    bool StringizeService::stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr )
    {
        const ConstStringHolder::value_type * holder_str = _holder->c_str();
        ConstStringHolder::size_type holder_size = _holder->size();
        ConstStringHolder::hash_type holder_hash = _holder->hash();

        if( holder_size == 0 )
        {
            _cstr = ConstString::none();

            return false;
        }

        if( holder_size == MENGINE_STRINGIZE_UNKNOWN_SIZE )
        {
            holder_size = (ConstStringHolder::size_type)strlen( holder_str );
        }

        if( holder_hash == MENGINE_STRINGIZE_UNKNOWN_HASH )
        {
            holder_hash = Helper::makeHash( holder_str, holder_size );
        }

        _holder->setup( holder_str, holder_size, holder_hash );

        ConstStringHolder * test = this->testHolder_( holder_str, holder_size, holder_hash );

        if( test != nullptr )
        {
            _cstr = ConstString( test );

            return false;
        }

        this->addHolder_( _holder, holder_hash );

        _cstr = ConstString( _holder );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder * StringizeService::stringizeHolder_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash )
    {
        ConstStringHolder * test = this->testHolder_( _str, _size, _hash );

        if( test != nullptr )
        {
            return test;
        }

        ConstStringHolderMemory * holder = m_poolHolderStringMemory.createT();

        holder->setValue( _str, _size, _hash );

        this->addHolder_( holder, _hash );

        return holder;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder * StringizeService::stringizeHolderUnique_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash )
    {
        ConstStringHolder * test = this->testHolder_( _str, _size, _hash );

        if( test != nullptr )
        {
            MENGINE_ASSERTION_FATAL( strcmp( test->c_str(), _str ) == 0, ("stringize unique '%s' alredy exist"
                , _str
                , test->c_str()
                ) );

            return test;
        }

        ConstStringHolderMemory * holder = m_poolHolderStringMemory.createT();

        holder->setValue( _str, _size, _hash );

        this->addHolder_( holder, _hash );

        return holder;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder * StringizeService::testHolder_( const Char * _str, ConstStringHolder::size_type _size, ConstString::hash_type _hash )
    {
        IntrusiveListConstStringHolder & list = this->getList_( _hash );

        for( IntrusiveListConstStringHolder::iterator
            it = list.begin(),
            it_end = list.end();
            it != it_end;
            ++it )
        {
            ConstStringHolder * holder = *it;

            ConstStringHolder::hash_type holder_hash = holder->hash();

            if( (int32_t)holder_hash != (int32_t)_hash )
            {
                continue;
            }

            ConstStringHolder::hash_type holder_size = holder->size();

            if( holder_size != _size )
            {
                continue;
            }

            const ConstStringHolder::value_type * holder_str = holder->c_str();

            if( strncmp( holder_str, _str, _size ) != 0 )
            {
                continue;
            }

            return holder;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void StringizeService::addHolder_( ConstStringHolder * _holder, ConstString::hash_type _hash )
    {
        IntrusiveListConstStringHolder & list = this->getList_( _hash );

        list.push_back( _holder );
    }
    //////////////////////////////////////////////////////////////////////////
    StringizeService::IntrusiveListConstStringHolder & StringizeService::getList_( ConstStringHolder::hash_type _hash )
    {
        uint32_t branch = (uint32_t)_hash % 4096;
        IntrusiveListConstStringHolder & list = m_holdres[branch];

        return list;
    }
}
