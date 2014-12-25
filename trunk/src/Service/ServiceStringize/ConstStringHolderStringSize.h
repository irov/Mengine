#   pragma once

#	include "Core/Hash.h"

#   include "stdex/const_string_holder.h"

#	include "stdex/memorycopy.h"

namespace Menge
{
	template<size_t Size> 
	class StringSizeBuffer
		: public Factorable
	{
	public:
		StringSizeBuffer()
		{
		}

		~StringSizeBuffer()
		{
		}

	public:
		const char * initialize( const char * _value, size_t _size )
		{
			stdex::memorycopy( m_buff, 0, _value, _size );
			m_buff[_size] = '\0';

			return m_buff;
		}

	protected:
		char m_buff[Size];
	};

    class ConstStringHolderStringSize
        : public stdex::const_string_holder
        , public Factorable
    {
	public:
		ConstStringHolderStringSize()
			: m_store(nullptr)
		{
		}

	public:
        void setValue( Factorable * _store, const char * _value, size_t _size )
        {			
			m_store = _store;

			hash_type hash = Helper::makeHash( _value, _size );

            this->setup( _value, _size, hash, true );
        }

    protected:
        void _releaseString() override
        {
			m_store->destroy();
			m_store = nullptr;
        }

    protected:
        void _destroyString() override
        {
			if( m_store != nullptr )
			{
				m_store->destroy();
				m_store = nullptr;
			}

            this->destroy();
        }

    protected:			
        Factorable * m_store;
    };
}