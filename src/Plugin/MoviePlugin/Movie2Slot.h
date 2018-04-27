#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
    class Movie2;

	class Movie2Slot
		: public Node
	{
    public:
        Movie2Slot();
        ~Movie2Slot() override;

    public:
        void setMovie( Movie2 * _movie );
        Movie2 * getMovie() const;

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
		void _debugRender( Mengine::RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask ) override;

    protected:
        Movie2 * m_movie;
	};
}
