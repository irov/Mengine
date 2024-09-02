#include "PhysicalPlaceholder.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PhysicalPlaceholder::PhysicalPlaceholder()
    {
        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    PhysicalPlaceholder::~PhysicalPlaceholder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PhysicalPlaceholder::setBox2DBody( const Box2DBodyInterfacePtr & _body )
    {
        m_body = _body;

        m_body->setNode( NodePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const Box2DBodyInterfacePtr & PhysicalPlaceholder::getBox2DBody() const
    {
        return m_body;
    }
    //////////////////////////////////////////////////////////////////////////
    void PhysicalPlaceholder::updateLocalMatrix() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_body );

        //always update local matrix
        this->invalidateLocalMatrix();

        mt::vec2f position = m_body->getBodyPosition();
        float angle = m_body->getBodyAngle();

        mt::vec3f new_position = m_position + mt::vec3f( position, 0.f );
        mt::vec3f new_origin = m_origin;
        mt::vec3f new_scale = m_scale;
        mt::vec2f new_skew = m_skew;
        mt::vec3f new_orientation = m_orientation + mt::vec3f( angle, 0.f, 0.f );

        BaseTransformation::makeLocalMatrix( &m_localMatrix, TRANSFORMATION_INVALIDATE_ALL, new_position, new_origin, new_scale, new_skew, new_orientation );

        m_transformationFlag = TRANSFORMATION_INVALIDATE_ALL;
    }
    //////////////////////////////////////////////////////////////////////////
}