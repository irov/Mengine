#include "MovieSceneEffect.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    bool MovieSceneEffect::setPropagateNode( Node * _propagate )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _propagate, false, "'%s' invalid set propagate node 'nullptr'"
            , this->getName().c_str()
        );

        m_propagate = _propagate;

        if( m_propagate->getRender() == nullptr )
        {
            LOGGER_ERROR( "'%s' propagate node '%s' type '%s' is not Renderable!"
                , this->getName().c_str()
                , m_propagate->getName().c_str()
                , m_propagate->getType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::invalidateColor() const
    {
        if( m_propagate == nullptr )
        {
            return;
        }

        const Color & personal_color = this->getPersonalColor();

        RenderInterface * render = m_propagate->getRender();

        render->setLocalColor( personal_color );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSceneEffect::_invalidateWorldMatrix() const
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
    void MovieSceneEffect::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
}