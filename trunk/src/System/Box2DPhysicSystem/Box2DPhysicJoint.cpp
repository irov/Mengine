#	include "Box2DPhysicJoint.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicJoint::Box2DPhysicJoint( b2World* _world, b2Joint* _joint, bool _isMouse )
        : m_joint( _joint )
        , m_world( _world )
        , m_isMouse( _isMouse )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    b2Joint* Box2DPhysicJoint::getJoint()
    {
        return m_joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicJoint::~Box2DPhysicJoint()
    {
        if( m_joint )
        {
            m_world->DestroyJoint( m_joint );
            m_joint = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////	
    void Box2DPhysicJoint::_setJoint(b2Joint *_joint)
    {
        m_joint = _joint;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicJoint::isMouse() const
    {
        return m_isMouse;
    }
    //////////////////////////////////////////////////////////////////////////
}