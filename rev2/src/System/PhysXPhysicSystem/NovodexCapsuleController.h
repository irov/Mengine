#	pragma once

#	include "Interface/PhysicSystemInterface.h"

#	include <NxCapsuleController.h>

class	NovodexCapsuleController : public ControllerInterface
{
public:
	NovodexCapsuleController( NxCapsuleController * _controller );
public:
	void setPosition( float * _pos ) override;
	void move( float * _displacement ) override;
	float * getFilteredPosition() override;
private:
	NxCapsuleController * m_controller;
};