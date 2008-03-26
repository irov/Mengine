#	include "Box2DPhysicJoint.h"

//////////////////////////////////////////////////////////////////////////
Box2DPhysicJoint::Box2DPhysicJoint( b2World* _world, b2Joint* _joint )
: m_joint( _joint )
, m_world( _world )
{

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