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
        if( this->isValid() == true )
        {
            ::b2DestroyJoint( m_jointId );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DJoint::initialize( const Box2DScaler & _scaler, b2JointId _jointId )
    {
        m_scaler = _scaler;
        m_jointId = _jointId;

        ::b2Joint_SetUserData( m_jointId, this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DJoint::isValid() const
    {
        return B2_IS_NON_NULL( m_jointId ) && ::b2Joint_IsValid( m_jointId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DJoint::setMouseTarget( const mt::vec2f & _target )
    {
        if( this->isValid() == false || ::b2Joint_GetType( m_jointId ) != b2_mouseJoint )
        {
            return false;
        }

        b2Vec2 target = m_scaler.toBox2DWorld( _target );

        ::b2MouseJoint_SetTarget( m_jointId, target );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DJoint::getMouseTarget() const
    {
        if( this->isValid() == false || ::b2Joint_GetType( m_jointId ) != b2_mouseJoint )
        {
            return mt::vec2f::identity();
        }

        b2Vec2 target = ::b2MouseJoint_GetTarget( m_jointId );

        return m_scaler.toEngineWorld( target );
    }
    //////////////////////////////////////////////////////////////////////////
    b2JointId Box2DJoint::getJointId() const
    {
        return m_jointId;
    }
    //////////////////////////////////////////////////////////////////////////
}
