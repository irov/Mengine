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

        const char * data = m_value.c_str();

        this->setup( data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderStringSTL::_releaseString()
    {
        String empty;
        m_value.swap(empty);
    }
}