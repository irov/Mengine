#	pragma once

#	include "NodeFactory.h"

using namespace Menge;

#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR(C)\
	Node * C::genObject(const std::string &_type){\
	Node *node = new C();\
	node->setType(_type);\
	return node;\
	}
#	else
#	define OBJECT_IMPLEMENT_GENERATOR(C)
#	endif


#	define OBJECT_IMPLEMENT_FACTORY(C)\
	OBJECT_IMPLEMENT_GENERATOR(C)\
	namespace {\
	volatile static NodeFactory::\
	Auto _classRegistration(#C,C::genObject);\
	}

#	define OBJECT_IMPLEMENT(C)\
	OBJECT_IMPLEMENT_FACTORY(C)



