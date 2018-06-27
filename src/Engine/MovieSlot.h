#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
	class MovieSlot
		: public Node
	{
    public:
        void setMovieName( const ConstString & _movieName );

    protected:
        void _destroy() override;

    protected:
        void _setLocalHide( bool _value ) override;

    protected:
        void _setPersonalColor( const ColourValue& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

	protected:
		void _changeParent( Node * _oldParent, Node * _newParent ) override;

	protected:
		void _debugRender( const RenderContext * _state ) override;

    protected:
        ConstString m_movieName;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieSlot> MovieSlotPtr;
    //////////////////////////////////////////////////////////////////////////
}
