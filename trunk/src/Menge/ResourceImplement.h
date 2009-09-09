#	pragma once

#	include "ResourceDeclare.h"
#	include "ResourceVisitor.h"

#	include "FactoryPool.h"

#	define RESOURCE_IMPLEMENT( Class )\
	Factory * Class::genFactory(){\
		return new FactoryPool<Class>();\
	}\
	void Class::accept( ResourceVisitor * _visitor ){ _visitor->visit(this); };\

//namespace {\
//volatile static TFactoryResource::\
//Auto _classRegistration(#C,C::genObject);\
//}
