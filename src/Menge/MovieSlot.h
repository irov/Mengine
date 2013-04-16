#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class MovieSlot
		: public Node
	{
    protected:
        void _localHide( bool _value ) override;

    protected:
        void _setPersonalColor( const ColourValue& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;
	};
}