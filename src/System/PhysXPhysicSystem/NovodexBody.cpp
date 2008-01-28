#	include "NovodexBody.h"	
//////////////////////////////////////////////////////////////////////////
NovodexBody::NovodexBody(NxActor * _actor) 
	: m_actor(_actor)
{}
//////////////////////////////////////////////////////////////////////////
NovodexBody::~NovodexBody() 
{}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::setOrient( float _w, float _x, float _y, float _z )
{
	m_actor->setGlobalOrientationQuat( NxQuat( NxVec3(_x, _y, _z), _w ) );
}
//////////////////////////////////////////////////////////////////////////
float * NovodexBody::getPosition()
{
	m_position = m_actor->getGlobalPosition();
	return m_position.get();
}
//////////////////////////////////////////////////////////////////////////
float * NovodexBody::getOrient()
{
	NxMat33 M = m_actor->getGlobalOrientation();
	M.getRowMajor(m_rot);

	static float orient[4] = {};
	m_actor->getGlobalOrientationQuat().getWXYZ(orient);
	return orient;
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::setActive( bool _active )
{
	if ( _active == true )
	{
		m_actor->wakeUp();
	}
	else
	{
		m_actor->putToSleep();
	}
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::applyImpulse( float _x, float _y, float _z ) 
{
	NxVec3 v = m_actor->getLinearMomentum();
	v.x += _x;	
	v.y += _y;	
	v.z += _z;
	m_actor->setLinearMomentum(v);
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::applyForce( float _x, float _y, float _z )
{
	m_actor->addForceAtPos(NxVec3(_x, _y, _z), NxVec3(0,0,0));
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::applyAngularImpulse( float _x, float _y, float _z )
{
	NxVec3 v = m_actor->getAngularMomentum();
	v.x += _x;
	v.y += _y;
	v.z += _z;
	m_actor->setAngularMomentum(v);
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::applyTorque( float _x, float _y, float _z )
{
	m_actor->addTorque( NxVec3(_x, _y, _z) );
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::setLinearVelocity( float _x, float _y, float _z )
{
	m_actor->setLinearVelocity( NxVec3(_x, _y, _z) );
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::setAngularVelocity( float _x, float _y, float _z )
{
	m_actor->setAngularVelocity( NxVec3(_x, _y, _z) );
}
//////////////////////////////////////////////////////////////////////////
void NovodexBody::setPosition( float _x, float _y, float _z ) 
{
	m_actor->setGlobalPosition( NxVec3(_x, _y, _z) );
}
//////////////////////////////////////////////////////////////////////////
NxActor * NovodexBody::getNxActor() const
{
	return m_actor;
}
//////////////////////////////////////////////////////////////////////////