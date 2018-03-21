#   include "ConstStringHolderMemory.h"

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
	void ConstStringHolderMemory::setValue( const char * _value, size_t _size, hash_type _hash )
	{
		m_buff = Helper::allocateMemoryT<char>( _size + 1 );
		stdex::memorycopy( m_buff, 0, _value, _size );
		m_buff[_size] = '\0';

		this->setup( m_buff, _size, _hash );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConstStringHolderMemory::destroyString()
	{
		Helper::freeMemory( m_buff );
		m_buff = nullptr;

        this->destroy();
	}
}