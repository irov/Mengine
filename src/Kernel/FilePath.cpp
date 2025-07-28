#include "FilePath.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const FilePath & _left, const FilePath::value_type * _right ) MENGINE_NOEXCEPT
    {
        const ConstString::value_type * left_str = _left.c_str();

        if( StdString::strcmp( left_str, _right ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const FilePath::value_type * _left, const FilePath & _right ) MENGINE_NOEXCEPT
    {
        const ConstString::value_type * right_str = _right.c_str();

        if( StdString::strcmp( _left, right_str ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const FilePath & _left, const FilePath::value_type * _right ) MENGINE_NOEXCEPT
    {
        const FilePath::value_type * left_str = _left.c_str();

        if( StdString::strcmp( left_str, _right ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const FilePath::value_type * _left, const FilePath & _right ) MENGINE_NOEXCEPT
    {
        const FilePath::value_type * right_str = _right.c_str();

        if( StdString::strcmp( _left, right_str ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}