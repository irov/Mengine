#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/DummyPicker.h"

namespace Mengine
{
    class MovieSlot
        : public Node
        , public BaseRender
        , public DummyPicker
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();

    public:
        MovieSlot();
        ~MovieSlot() override;

    public:
        void setMovieName( const ConstString & _movieName );

    protected:
        void _destroy() override;

    protected:
        void _setLocalHide( bool _value ) override;

    protected:
        void _setPersonalColor( const Color& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void _changeParent( Node * _oldParent, Node * _newParent ) override;

    protected:
        void render( const RenderContext * _context ) const override;

    protected:
        ConstString m_movieName;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieSlot> MovieSlotPtr;
    //////////////////////////////////////////////////////////////////////////
}
