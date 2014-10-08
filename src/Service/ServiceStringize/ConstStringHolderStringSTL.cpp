#   include "ConstStringHolderStringSTL.h"

#	include "Core/Memory.h"

#	include "stdex/memorycopy.h"

#	include "Core/Hash.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderStringSTL::ConstStringHolderStringSTL()
		: m_buff(nullptr)
    {    
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::setValue( const char * _value, size_t _size )
    {
		m_buff = Helper::allocateMemory<char>( _size + 1 );

		stdex::memorycopy( m_buff, _value, _size );
		m_buff[_size] = '\0';

		hash_type hash = Helper::makeHash( m_buff, _size );

        this->setup( m_buff, _size, hash, true );
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