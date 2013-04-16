#   include "ConstStringHolderString256.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderString256::ConstStringHolderString256()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderString256::setValue( const char * _value, size_t _size )
    {
        memcpy( m_value, _value, _size );
        m_value[_size] = 0;

        this->setup( m_value, _size );
    }
}