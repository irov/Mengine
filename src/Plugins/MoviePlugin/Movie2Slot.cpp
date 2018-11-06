#include "Movie2Slot.h"
#include "Movie2.h"

#include "Kernel/Logger.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot::Movie2Slot()
        : m_movie( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot::~Movie2Slot()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::setMovie( const Movie2Ptr & _movie )
    {
        m_movie = _movie;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2Ptr & Movie2Slot::getMovie() const
    {
        return m_movie;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_destroy()
    {
#ifndef NDEBUG
        IntrusiveSlugListNodeChild & child = this->getChildren();

        for( IntrusiveSlugListNodeChild::iterator
            it = child.begin(),
            it_end = child.end();
            it != it_end;
            ++it )
        {
            NodePtr node = *it;

            LOGGER_ERROR( "Movie2Slot::_destroy slot %s:%s has children %s:%s!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , node->getName().c_str()
                , node->getType().c_str()
                , m_movie->getName().c_str()
            );
        }
#endif

        Node::_destroy();
    }
    ////////////////////////////////////////////////////////////////////////////
    //void Movie2Slot::_setLocalHide( bool _value )
    //{
    //    Node::_setLocalHide( _value );

    //    this->setHide( _value );
    //}
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_setPersonalColor( const ColourValue& _color )
    {
        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_setPersonalAlpha( float _alpha )
    {
        this->setLocalAlpha( _alpha );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_changeParent( Node * _oldParent, Node * _newParent )
    {
        (void)_newParent;

        if( _oldParent == nullptr )
        {
            return;
        }

        if( _newParent == nullptr )
        {
            return;
        }

        LOGGER_ERROR( "MovieSlot::_changeParent slot %s:%s movie %s (please don't change parent it's danger)"
            , this->getName().c_str()
            , this->getType().c_str()
            , m_movie->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_render( const RenderContext * _state )
    {
        (void)_state;

        //Empty
    }
}
