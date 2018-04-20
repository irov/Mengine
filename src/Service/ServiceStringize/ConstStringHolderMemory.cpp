#include "ConstStringHolderMemory.h"

#include "Core/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ConstStringHolderMemory::ConstStringHolderMemory()
		: m_buff( nullptr )
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderMemory::~ConstStringHolderMemory()
    {
        if( m_buff != nullptr )
        {
            Helper::freeMemoryT( m_buff );
            m_buff = nullptr;
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void ConstStringHolderMemory::setValue( const Char * _value, size_t _size, hash_type _hash )
	{
		m_buff = Helper::allocateMemoryT<Char>( _size + 1 );
		stdex::memorycopy( m_buff, 0, _value, _size );
		m_buff[_size] = '\0';

		this->setup( m_buff, _size, _hash );
	}
}