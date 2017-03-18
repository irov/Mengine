#	include "StringizeService.h"

#	include "Core/Hash.h"

#   include "Factory/FactoryPool.h"

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
	bool StringizeService::_initialize()
	{
		m_factoryHolderStringMemory = new FactoryPool<ConstStringHolderMemory, 512>(m_serviceProvider);

		for (uint32_t i = 0; i != 257; ++i)
		{
			for (uint32_t j = 0; j != 8; ++j)
			{
				InternalHolder & holder = m_internals[i][j];

				holder.str = nullptr;
				holder.holder = nullptr;				
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void StringizeService::_finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void StringizeService::stringize( const char * _str, size_t _size, ConstString::hash_type _hash, ConstString & _cstr )
	{
        if( _size == (size_t)-1 )
		{
			_size = strlen(_str);
		}

		if (_size == 0)
		{
			_cstr = ConstString::none();

			return;
		}

		if( _hash == (ConstString::hash_type)(-1) )
        {
            _hash = Helper::makeHash( _str, _size );
        }

		ConstStringHolder * holder = this->stringizeHolder_(_str, _size, _hash);

		_cstr = ConstString(holder);
    }
	//////////////////////////////////////////////////////////////////////////
	void StringizeService::stringizeInternal(const Char * _str, ConstString::size_type _size, ConstString & _cstr)
	{
		if (_size == 0)
		{
			_cstr = ConstString::none();

			return;
		}

		ptrdiff_t ptr_diff = reinterpret_cast<ptrdiff_t>(_str);

		uint32_t ptr_hash = (ptr_diff / sizeof(ptrdiff_t)) % 257;

		for (uint32_t i = 0; i != 8; ++i)
		{
			InternalHolder & inter = m_internals[ptr_hash][i];

			if (inter.str == _str)
			{
				_cstr = ConstString(inter.holder);

				return;
			}

			if (inter.holder != nullptr)
			{
				continue;
			}

			ConstString::hash_type _hash = Helper::makeHash(_str, _size);
			
			ConstStringHolder * holder = this->stringizeHolder_(_str, _size, _hash);

			inter.str = _str;
			inter.holder = holder;

			_cstr = ConstString(holder);

			return;
		}

		ConstString::hash_type hash = Helper::makeHash(_str, _size);

		ConstStringHolder * holder = this->stringizeHolder_(_str, _size, hash);

		_cstr = ConstString(holder);
	}
    //////////////////////////////////////////////////////////////////////////
    bool StringizeService::stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr )
    {
        const ConstStringHolder::value_type * holder_str = _holder->c_str();
        ConstStringHolder::size_type holder_size = _holder->size();
        ConstStringHolder::hash_type holder_hash = _holder->hash();

		if( holder_size == (ConstStringHolder::size_type)(-1) )
        {
            holder_size = strlen( holder_str );
        }

		if (holder_size == 0)
		{
			_cstr = ConstString::none();
		}

		if( holder_hash == (ConstStringHolder::hash_type) (-1) )
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

		_cstr = ConstString(_holder);

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	ConstStringHolder * StringizeService::stringizeHolder_(const char * _str, size_t _size, ConstString::hash_type _hash)
	{
		ConstStringHolder * test = this->testHolder_(_str, _size, _hash);

		if (test != nullptr)
		{
			return test;
		}

		ConstStringHolderMemory * holder = m_factoryHolderStringMemory->createObject();

		holder->setValue(_str, _size, _hash);

		this->addHolder_(holder, _hash);

		return holder;
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

			if( (int32_t)holder_hash != (int32_t)_hash )
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
		TIntrusiveListConstStringHolder & list = this->getList_( _hash );

        list.push_back( _holder );
    }
    //////////////////////////////////////////////////////////////////////////
    StringizeService::TIntrusiveListConstStringHolder & StringizeService::getList_( ConstStringHolder::hash_type _hash )
    {
        uint32_t branch = (uint32_t)_hash % 4096;
        TIntrusiveListConstStringHolder & list = m_holdres[branch];

        return list;
    }
}
