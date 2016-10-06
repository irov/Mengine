#	include "StringizeService.h"

#	include "Core/Hash.h"

#	include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StringizeService, Menge::StringizeService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
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
	void StringizeService::stringize( const char * _str, size_t _size, ConstString::hash_type _hash, ConstString & _cstr )
	{
		if( _size == (size_t)-1 )
		{
			_size = strlen(_str);
		}

        if( _hash == (size_t)-1 )
        {
            _hash = Helper::makeHash( _str, _size );
        }

        ConstStringHolder * test = this->testHolder_( _str, _size, _hash );

        if( test != nullptr )
        {
            _cstr = ConstString( test );

            return;
        }

        ConstStringHolderMemory * holder = m_poolHolderStringMemory.createObject();

        holder->setValue( _str, _size, _hash );

        this->addHolder_( holder, _cstr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool StringizeService::stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr )
    {
        const ConstStringHolder::value_type * holder_str = _holder->c_str();
        ConstStringHolder::size_type holder_size = _holder->size();
        ConstStringHolder::hash_type holder_hash = _holder->hash();

        if( holder_size == (size_t)-1 )
        {
            holder_size = strlen( holder_str );
        }

        if( holder_hash == (size_t)-1 )
        {
            holder_hash = Helper::makeHash( holder_str, holder_size );
        }

        ConstStringHolder * test = this->testHolder_( holder_str, holder_size, holder_hash );
        
        if( test != nullptr )
        {
            _cstr = ConstString( test );

            return false;
        }

        this->addHolder_( _holder, _cstr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder * StringizeService::testHolder_( const char * _str, ConstString::size_type _size, ConstString::hash_type _hash )
    {
        TIntrusiveListConstStringHolder & list = this->getList_( _hash );

        for( TIntrusiveListConstStringHolder::iterator
            it = list.begin(),
            it_end = list.end();
            it != it_end;
            ++it )
        {
            ConstStringHolder * holder = *it;

            ConstStringHolder::hash_type holder_hash = holder->hash();

            if( holder_hash != _hash )
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
    void StringizeService::addHolder_( ConstStringHolder * _holder, ConstString & _cstr )
    {
        ConstStringHolder::hash_type holder_hash = _holder->hash();

        TIntrusiveListConstStringHolder & list = this->getList_( holder_hash );

        list.push_back( _holder );

        _cstr = ConstString( _holder );
    }
    //////////////////////////////////////////////////////////////////////////
    StringizeService::TIntrusiveListConstStringHolder & StringizeService::getList_( ConstStringHolder::hash_type _hash )
    {
        uint32_t branch = (uint32_t)_hash % 4096;
        TIntrusiveListConstStringHolder & list = m_holdres[branch];

        return list;
    }
}
