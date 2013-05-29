#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class MovieSlot
		: public Node
	{
    public:
        void setMovieName( const ConstString & _movieName );

    protected:
        bool _destroy() override;

    protected:
        void _localHide( bool _value ) override;

    protected:
        void _setPersonalColor( const ColourValue& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        ConstString m_movieName;
	};
}