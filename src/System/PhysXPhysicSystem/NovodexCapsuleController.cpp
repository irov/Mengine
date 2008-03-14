#	include "NovodexCapsuleController.h"

NovodexCapsuleController::NovodexCapsuleController( NxCapsuleController * _controller )
	: m_controller(_controller)
{};

void NovodexCapsuleController::setPosition( float * _pos )
{
	m_controller->setPosition( NxExtendedVec3( _pos[0], _pos[1], _pos[2] ) );
}

void NovodexCapsuleController::move( float * _displacement )
{
	NxVec3 d = *(NxVec3*)_displacement;
	//d+=NxVec3(0,-9.8f,0);
	NxU32 collisionFlags;
	m_controller->move( d, 6, 0.0001f, collisionFlags, 1.0f );
}

float * NovodexCapsuleController::getFilteredPosition()
{
	static	NxExtendedVec3 v;
	static float p[3];
	v = m_controller->getPosition();
	p[0] = v.x;
	p[1] = v.y;
	p[2] = v.z;
	return  p;
}