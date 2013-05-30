#   include "ConstStringHolderWStringSTL.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderWStringSTL::ConstStringHolderWStringSTL()
    {    
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderWStringSTL::setValue( const wchar_t * _value, size_t _size )
    {
        m_value.assign( _value, _size );

        const wchar_t * data = m_value.c_str();

        this->setup( data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderWStringSTL::releaseString()
    {
        WString empty;
        m_value.swap(empty);
    }
}