#include "Movie2Slot.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot::Movie2Slot()
        : m_layerIndex( ~0U )
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
    void Movie2Slot::setCompositionName( const ConstString & _compositionName )
    {
        m_compositionName = _compositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2Slot::getCompositionName() const
    {
        return m_compositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::setLayerIndex( uint32_t _layerIndex )
    {
        m_layerIndex = _layerIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Movie2Slot::getLayerIndex() const
    {
        return m_layerIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::setDimension( const Optional<mt::box2f> & _dimension )
    {
        m_dimension = _dimension;
    }
    //////////////////////////////////////////////////////////////////////////
    const Optional<mt::box2f> & Movie2Slot::getDimension() const
    {
        return m_dimension;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::setOptions( const UInt32s & _options )
    {
        m_options = _options;
    }
    //////////////////////////////////////////////////////////////////////////
    const UInt32s & Movie2Slot::getOptions() const
    {
        return m_options;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2Slot::hasOption( const Char * _option4 ) const
    {
        ae_uint32_t option = ae_make_option( _option4 );

        if( Algorithm::find( m_options.begin(), m_options.end(), option ) == m_options.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_destroy()
    {
#if defined(MENGINE_DEBUG)
        this->foreachChildrenSlug( [this]( const NodePtr & _child )
        {
            LOGGER_ERROR( "movie2 slot '%s' type '%s' has children name '%s' type '%s'!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , _child->getName().c_str()
                , _child->getType().c_str()
                , this->getMovieName().c_str()
            );
        } );
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
    void Movie2Slot::_hierarchyChangeParent( Node * _oldParent, Node * _newParent )
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

        LOGGER_ERROR( "movie2 slot '%s' type '%s' movie '%s' (please don't change parent it's danger)"
            , this->getName().c_str()
            , this->getType().c_str()
            , this->getMovieName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
}
