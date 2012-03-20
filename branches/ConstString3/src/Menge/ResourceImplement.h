#	pragma once

#	include "ResourceDeclare.h"
#	include "ResourceVisitor.h"

#	include "FactoryPool.h"

//Factory * Class::genFactory(){\
//	return new FactoryDefault<Class>();\
//}

#	define RESOURCE_IMPLEMENT( Class )\
	void Class::accept( ResourceVisitor * _visitor ){ _visitor->visit(this); }\
	static bool _dummy_dot_##Class

