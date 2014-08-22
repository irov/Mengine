#   include "ConstStringHolderStringSTL.h"

#	include "stdex/memorycopy.h"

#   include <memory.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderStringSTL::ConstStringHolderStringSTL()
    {    
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::setValue( const char * _value, size_t _size )
    {
		m_buff.resize( _size + 1 );

		stdex::memorycopy( &m_buff[0], _value, _size );
		m_buff[_size] = '\0';

        this->setup( &m_buff[0], _size, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::_releaseString()
    {
		TVectorChar deleter;
		m_buff.swap( deleter );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::_destroyString()
    {
		TVectorChar deleter;
		m_buff.swap( deleter );

        this->destroy();
    }
}