#   pragma once

#   include "stdex/const_string_holder.h"

#   include <memory.h>

namespace Menge
{
	template<size_t Size> 
	class StringSizeBuffer
		: public Factorable
	{
	public:
		const char * initialize( const char * _value, size_t _size )
		{
			memcpy( m_buff, _value, _size );
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

            this->setup( _value, _size );
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