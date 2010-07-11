#	pragma once

#	include "ResourceDeclare.h"
#	include "ResourceVisitor.h"

#	include "FactoryPool.h"

#	define RESOURCE_IMPLEMENT( Class )\
	Factory * Class::genFactory(){\
		return new FactoryPool<Class>();\
	}\
	void Class::accept( ResourceVisitor * _visitor ){ _visitor->visit(this); }\
	class _dummy_dot_##Class{}

