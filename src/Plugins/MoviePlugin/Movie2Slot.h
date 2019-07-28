#pragma once

#include "Kernel/Node.h"
#include "Kernel/DummyRender.h"
#include "Kernel/DummyPicker.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2Slot
        : public Node
        , public DummyRender
        , public DummyPicker
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();

    public:
        Movie2Slot();
        ~Movie2Slot() override;

    public:
        void setMovieName( const ConstString & _movieName );
        const ConstString & getMovieName() const;

    protected:
        void _destroy() override;

    protected:
        void _setPersonalColor( const Color& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void _changeParent( Node * _oldParent, Node * _newParent ) override;

    protected:
        ConstString m_movieName;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2Slot> Movie2SlotPtr;
    //////////////////////////////////////////////////////////////////////////
}
