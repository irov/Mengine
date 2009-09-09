#	include "NovodexCapsuleController.h"
#	include "NovodexPhysicSystem.h"
//////////////////////////////////////////////////////////////////////////
NovodexCapsuleController::NovodexCapsuleController( NxCapsuleController * _controller )
	: m_controller(_controller)
{}
//////////////////////////////////////////////////////////////////////////
void NovodexCapsuleController::setPosition( float _x, float _y, float _z )
{
	m_controller->setPosition( NxExtendedVec3( _x, _y, _z ) );
}
//////////////////////////////////////////////////////////////////////////
void NovodexCapsuleController::move( float _x, float _y, float _z )
{
	NxU32 collisionFlags;
	m_controller->move( NxVec3(_x, _y, _z), COLLIDABLE_MASK, 0.0001f, collisionFlags, 1.0f );
}
//////////////////////////////////////////////////////////////////////////
const double * NovodexCapsuleController::getFilteredPosition() const
{
	return &(m_controller->getPosition().x);
}
//////////////////////////////////////////////////////////////////////////