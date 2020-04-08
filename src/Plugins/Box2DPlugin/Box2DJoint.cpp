#include "Box2DJoint.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DJoint::Box2DJoint()
        : m_world( nullptr )
        , m_joint( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJoint::~Box2DJoint()
    {
        if( m_world != nullptr && m_joint != nullptr )
        {
            m_world->DestroyJoint( m_joint );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DJoint::initialize( b2World * _world, const b2JointDef * _jointDef )
    {
        b2Joint * b2_joint = _world->CreateJoint( _jointDef );

        MENGINE_ASSERTION_MEMORY_PANIC( b2_joint, false );

        b2_joint->SetUserData( this );

        m_world = _world;
        m_joint = b2_joint;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    b2World * Box2DJoint::getWorld() const
    {
        return m_world;
    }
    //////////////////////////////////////////////////////////////////////////
    b2Joint * Box2DJoint::getJoint() const
    {
        return m_joint;
    }
    //////////////////////////////////////////////////////////////////////////
}