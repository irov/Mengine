#include "ConstString.h"
#include "ConstStringHolder.h"

#include <string.h>

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

        if( strcmp( left_str, _right ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const ConstString::value_type * _left, const ConstString & _right )
    {
        const ConstString::value_type * right_str = _right.c_str();

        if( strcmp( _left, right_str ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const ConstString & _left, const ConstString::value_type * _right )
    {
        const ConstString::value_type * left_str = _left.c_str();

        if( strcmp( left_str, _right ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const ConstString::value_type * _left, const ConstString & _right )
    {
        const ConstString::value_type * right_str = _right.c_str();

        if( strcmp( _left, right_str ) == 0 )
        {
            return false;
        }

        return true;
    }
}