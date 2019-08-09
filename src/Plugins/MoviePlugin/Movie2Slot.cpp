#include "Movie2Slot.h"

#include "Kernel/Logger.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot::Movie2Slot()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot::~Movie2Slot()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::setMovieName( const ConstString & _movieName )
    {
        m_movieName = _movieName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2Slot::getMovieName() const
    {
        return m_movieName;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_destroy()
    {
#ifdef MENGINE_DEBUG
        IntrusiveSlugListNodeChild & child = this->getChildren();

        for( IntrusiveSlugListNodeChild::iterator
            it = child.begin(),
            it_end = child.end();
            it != it_end;
            ++it )
        {
            NodePtr node = *it;

            LOGGER_ERROR( "slot '%s:%s' has children '%s:%s'!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , node->getName().c_str()
                , node->getType().c_str()
                , this->getMovieName().c_str()
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
    void Movie2Slot::_setPersonalColor( const Color & _color )
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
        MENGINE_UNUSED( _newParent );

        if( _oldParent == nullptr )
        {
            return;
        }

        if( _newParent == nullptr )
        {
            return;
        }

        LOGGER_ERROR( "slot '%s:%s' movie '%s' (please don't change parent it's danger)"
            , this->getName().c_str()
            , this->getType().c_str()
            , this->getMovieName().c_str()
        );
    }
}
