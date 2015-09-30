#   include "ConstStringHolderStringSTL.h"

#	include "Core/Memory.h"
#	include "Core/MemoryAllocator.h"

#	include "stdex/memorycopy.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderStringSTL::ConstStringHolderStringSTL()
		: m_buff(nullptr)
    {    
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::setValue( const char * _value, size_t _size, hash_type _hash )
    {
		m_buff = Helper::allocateMemory<char>( _size + 1 );

		stdex::memorycopy( m_buff, 0, _value, _size );
		m_buff[_size] = '\0';

        this->setup( m_buff, _size, _hash, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::_releaseString()
    {
		Helper::freeMemory( m_buff );
		m_buff = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::_destroyString()
    {
		Helper::freeMemory( m_buff );
		m_buff = nullptr;

        this->destroy();
    }
}