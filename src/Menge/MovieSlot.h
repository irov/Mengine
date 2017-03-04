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
        void _destroy() override;

    protected:
        void _localHide( bool _value ) override;

    protected:
        void _setPersonalColor( const ColourValue& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

	protected:
		void _changeParent( Node * _oldParent, Node * _newParent ) override;

	protected:
		void _debugRender( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask ) override;

    protected:
        ConstString m_movieName;
	};
}
