#   include "ConstStringHolderStringSTL.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderStringSTL::ConstStringHolderStringSTL()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::setValue( const char * _value, size_t _size )
    {
        m_value.assign( _value, _size );

        this->setup( _value, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::releaseString()
    {
        String empty;
        m_value.swap(empty);
    }
}