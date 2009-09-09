#	pragma once

#	include "Node.h"

#	include "FactoryPool.h"

#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR( Class )\
	Factory * Class::genFactory(){\
	return new FactoryPool<Class>();\
	}

#	else
#	define OBJECT_IMPLEMENT_GENERATOR(Class)
#	endif

#	define OBJECT_IMPLEMENT_FACTORY( Class )\
	OBJECT_IMPLEMENT_GENERATOR( Class )

#	define FACTORABLE_IMPLEMENT(Class)\
	OBJECT_IMPLEMENT_FACTORY(Class)



