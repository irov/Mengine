#include "Rect.h"

namespace Mengine
{
    /////////////////////////////////////////////////////////////////////////////////
    Rect::Rect()
        : left( 0 )
        , top( 0 )
        , right( 0 )
        , bottom( 0 )
    {
    }
    /////////////////////////////////////////////////////////////////////////////////
    Rect::Rect( uint32_t _left, uint32_t _top, uint32_t _right, uint32_t _bottom )
        : left( _left )
        , top( _top )
        , right( _right )
        , bottom( _bottom )
    {
    }
    /////////////////////////////////////////////////////////////////////////////////
    bool Rect::full( uint32_t _width, uint32_t _height ) const
    {
        if( left != 0 )
        {
            return false;
        }

        if( top != 0 )
        {
            return false;
        }

        if( right != _width )
        {
            return false;
        }

        if( bottom != _height )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}