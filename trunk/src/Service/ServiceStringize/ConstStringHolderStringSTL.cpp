#   include "ConstStringHolderStringSTL.h"

#   include <memory.h>

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
		m_buff = new char[_size + 1];

		memcpy( m_buff, _value, _size );
		m_buff[_size] = '\0';

        this->setup( m_buff, _size, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::_releaseString()
    {
        delete [] m_buff;
		m_buff = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::_destroyString()
    {
		delete [] m_buff;
		m_buff = nullptr;

        this->destroy();
    }
}