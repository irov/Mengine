#	include "ConstString.h"

#   include <string.h>

namespace Menge
{    
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ConstString::none()
    {
        static ConstString s_none( new ConstStringHolderNone() );

        return s_none;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const ConstString & _left, const char * _right )
    {
        const char * left_str = _left.c_str();

        if( strcmp( left_str, _right ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const char * _left, const ConstString & _right )
    {
        const char * right_str = _right.c_str();

        if( strcmp( _left, right_str ) != 0 )
        {
            return false;
        }

        return true;
    }
}