#	pragma once

#	include "FactoryDefault.h"

#	define FACTORABLE_IMPLEMENT( Class )\
	Factory * Class::genFactory(){\
	return new FactoryDefault<Class>();\
	}