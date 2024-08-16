#include "ConstString.h"
#include "ConstStringHolder.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ConstString::none()
    {
        static ConstString s_none;

        return s_none;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const ConstString & _left, const ConstString::value_type * _right )
    {
        const ConstString::value_type * left_str = _left.c_str();

        if( StdString::strcmp( left_str, _right ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const ConstString::value_type * _left, const ConstString & _right )
    {
        const ConstString::value_type * right_str = _right.c_str();

        if( StdString::strcmp( _left, right_str ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const ConstString & _left, const ConstString::value_type * _right )
    {
        const ConstString::value_type * left_str = _left.c_str();

        if( StdString::strcmp( left_str, _right ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const ConstString::value_type * _left, const ConstString & _right )
    {
        const ConstString::value_type * right_str = _right.c_str();

        if( StdString::strcmp( _left, right_str ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString::value_type * ConstString::begin() const
    {
        const value_type * holder_str = m_holder->c_str();

        return holder_str;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString::value_type * ConstString::end() const
    {
        const size_type holder_size = m_holder->size();
        const value_type * holder_str = m_holder->c_str();

        return holder_str + holder_size;
    }
    //////////////////////////////////////////////////////////////////////////
}