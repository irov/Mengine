#pragma once

#include "Kernel/Node.h"
#include "Kernel/DummyRender.h"

namespace Mengine
{    
    class Movie2Slot
        : public Node
        , public DummyRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        Movie2Slot();
        ~Movie2Slot() override;

    public:
        void setMovieName( const ConstString & _movieName );
        const ConstString & getMovieName() const;

    protected:
        void _destroy() override;

    protected:
        void _setPersonalColor( const Color & _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void _changeParent( Node * _oldParent, Node * _newParent ) override;

    protected:
        ConstString m_movieName;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Movie2Slot> Movie2SlotPtr;
    //////////////////////////////////////////////////////////////////////////
}
