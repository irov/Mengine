#include "PhysicalPlaceholder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PhysicalPlaceholder::PhysicalPlaceholder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PhysicalPlaceholder::~PhysicalPlaceholder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PhysicalPlaceholder::setBox2DBody( const Box2DBodyInterfacePtr & _body )
    {
        m_body = Box2DBodyPtr::from( _body );

        m_body->setEventable( EventablePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const Box2DBodyInterfacePtr & PhysicalPlaceholder::getBox2DBody() const
    {
        return m_body;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PhysicalPlaceholder::_activate()
    {
        this->invalidateLocalMatrix();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PhysicalPlaceholder::_deactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////	
    void PhysicalPlaceholder::updateLocalMatrix() const
    {
        MENGINE_ASSERTION( m_body != nullptr );

        //always update local matrix
        m_invalidateLocalMatrix = true; 

        mt::vec2f position = m_body->getPosition();
        float angle = m_body->getAngle();

        mt::vec3f new_position = m_position + mt::vec3f( position, 0.f );
        mt::vec3f new_origin = m_origin;
        mt::vec3f new_scale = m_scale;
        mt::vec2f new_skew = m_skew;
        mt::vec3f new_orientation = m_orientation + mt::vec3f( angle, 0.f, 0.f );

        Transformation::makeLocalMatrix_( m_localMatrix, TRANSFORMATION_INVALIDATE_UNKNOWN, new_position, new_origin, new_scale, new_skew, new_orientation );
    }
}