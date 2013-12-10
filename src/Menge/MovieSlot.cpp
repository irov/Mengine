#	include "MovieSlot.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::setMovieName( const ConstString & _movieName )
    {
        m_movieName = _movieName;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_destroy()
    {
        TListNodeChild & child = this->getChild();

        for( TListNodeChild::iterator
            it = child.begin(),
            it_end = child.end();
        it != it_end;
        ++it )
        {
            const Node * node = *it;

            LOGGER_ERROR(m_serviceProvider)("MovieSlot::_destroy slot %s:%s has children %s:%s!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , node->getName().c_str()
                , node->getType().c_str()
                , m_movieName.c_str()
                );
        }

        this->removeAllChild();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_localHide( bool _value )
    {
        this->hide( _value );
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
}