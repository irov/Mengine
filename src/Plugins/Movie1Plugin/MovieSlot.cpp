#include "MovieSlot.h"

#include "Kernel/Logger.h"

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
#ifdef MENGINE_DEBUG
        this->foreachChildrenSlug( [this]( const NodePtr & _child )
        {
            LOGGER_ERROR( "slot name '%s' type '%s' has children name '%s' type '%s'!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , _child->getName().c_str()
                , _child->getType().c_str()
                , m_movieName.c_str()
            );
        } );
#endif

        Node::_destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setLocalHide( bool _value )
    {
        this->setHide( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setPersonalColor( const Color & _color )
    {
        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setPersonalAlpha( float _alpha )
    {
        this->setLocalAlpha( _alpha );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_hierarchyChangeParent( Node * _oldParent, Node * _newParent )
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

        LOGGER_ERROR( "slot name '%s' type '%s' movie '%s' (please don't change parent it's danger)"
            , this->getName().c_str()
            , this->getType().c_str()
            , m_movieName.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
