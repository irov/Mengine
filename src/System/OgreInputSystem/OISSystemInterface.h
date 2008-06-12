#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "OIS/OIS.h"

class OgreInputSystemInterface
	: public InputSystemInterface
{
public:
	virtual bool init( const OIS::ParamList & _params ) = 0;
};