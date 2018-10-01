#include "MovieSceneEffect.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MovieSceneEffect::MovieSceneEffect()
        : m_propagate( nullptr )
        , m_propagateEnable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieSceneEffect::~MovieSceneEffect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::enablePropagate( bool _value )
    {
        m_propagateEnable = _value;

        if( m_propagateEnable == true )
        {
            uint8_t propagateTransformationFlag;
            m_propagate->getTransformation( propagateTransformationFlag, m_position, m_origin, m_scale, m_skew, m_rotation );

            mt::vec3f position;
            mt::vec3f origin;

            mt::vec3f scale;
            mt::vec2f skew;
            mt::vec3f rotation;

            uint8_t localeTransformationFlag;
            this->getTransformation( localeTransformationFlag, position, origin, scale, skew, rotation );

            m_propagate->setTransformation( TRANSFORMATION_INVALIDATE_UNKNOWN, position, origin, scale, skew, rotation );
        }
        else
        {
            m_propagate->setTransformation( m_transformationFlag, m_position, m_origin, m_scale, m_skew, m_rotation );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::setPropagateNode( Node * _propagate )
    {
        m_propagate = _propagate;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::_invalidateColor()
    {
        if( m_propagate == nullptr )
        {
            return;
        }

        const ColourValue & personal_colour = this->getPersonalColor();

        RenderInterfacePtr render = m_propagate->getRender();

        if( render != nullptr )
        {
            render->setLocalColor( personal_colour );
        }
        else
        {
            m_propagate->foreachRenderCloseChildren( [personal_colour]( const RenderInterfacePtr & _render )
            {
                _render->setLocalColor( personal_colour );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::_invalidateWorldMatrix()
    {
        if( m_propagate == nullptr )
        {
            return;
        }

        if( m_propagateEnable == false )
        {
            return;
        }

        uint8_t transformationFlag;

        mt::vec3f position;
        mt::vec3f origin;
        mt::vec3f scale;
        mt::vec2f skew;
        mt::vec3f rotation;

        this->getTransformation( transformationFlag, position, origin, scale, skew, rotation );

        m_propagate->setTransformation( transformationFlag, position, origin, scale, skew, rotation );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::_render( const RenderContext * _context )
    {
        (void)_context;

        //Empty
    }
}