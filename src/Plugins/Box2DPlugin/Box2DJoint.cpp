#include "Box2DJoint.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DJoint::Box2DJoint()
        : m_jointId( b2_nullJointId )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJoint::~Box2DJoint()
    {
        if( B2_IS_NON_NULL( m_jointId ) )
        {
            ::b2DestroyJoint( m_jointId );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DJoint::initialize( b2JointId _jointId )
    {
        m_jointId = _jointId;

        ::b2Joint_SetUserData( m_jointId, this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    b2JointId Box2DJoint::getJointId() const
    {
        return m_jointId;
    }
    //////////////////////////////////////////////////////////////////////////
}