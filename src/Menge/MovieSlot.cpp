#	include "MovieSlot.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_localHide( bool _value )
    {
        this->hide( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setPersonalColor( const ColourValue& _color )
    {
        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setPersonalAlpha( float _alpha )
    {
        this->setLocalAlpha( _alpha );
    }
}