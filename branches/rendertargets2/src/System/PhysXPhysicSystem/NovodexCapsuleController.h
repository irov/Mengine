#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include <NxCapsuleController.h>

class	NovodexCapsuleController : public Menge::ControllerInterface
{
public:
	NovodexCapsuleController( NxCapsuleController * _controller );
public:
	void setPosition( float _x, float _y, float _z ) override;
	void move( float _x, float _y, float _z ) override;
	const double * getFilteredPosition() const;
private:
	NxCapsuleController * m_controller;
};