#include "MovieSlot.h"

#include "Kernel/Logger.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MovieSlot::MovieSlot()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieSlot::~MovieSlot()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::setMovieName( const ConstString & _movieName )
    {
        m_movieName = _movieName;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_destroy()
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

            LOGGER_ERROR( "MovieSlot::_destroy slot %s:%s has children %s:%s!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , node->getName().c_str()
                , node->getType().c_str()
                , m_movieName.c_str()
            );
        }
#endif

        Node::_destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setLocalHide( bool _value )
    {
        this->setHide( _value );
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
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_changeParent( Node * _oldParent, Node * _newParent )
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
            , m_movieName.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_render( const RenderContext * _context )
    {
        (void)_context;

        //Empty
    }
}
