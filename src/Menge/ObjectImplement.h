#	pragma once

#	include "ScriptClassWrapper.h"
#	include "NodeFactory.h"
#	include "Visitor.h"

using namespace Menge;

#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR( Class )\
	Node * Class::genObject(const FactoryGenStruct & _struct){\
	Node * node = new Class();\
	ScriptClassWrapperFactory::wrapp( #Class, node );\
	node->setType(_struct.type);\
	return node;\
	}
#	else
#	define OBJECT_IMPLEMENT_GENERATOR(Class)
#	endif


#	define OBJECT_IMPLEMENT_FACTORY( Class )\
	OBJECT_IMPLEMENT_GENERATOR( Class )\
	namespace {\
	volatile static NodeFactory::\
	Auto _classRegistration(#Class,Class::genObject);\
	}

#	define OBJECT_IMPLEMENT_VISITOR(Class)\
	void Class::visit( Visitor * _visitor)\
	{\
		_visit(_visitor);\
		_visitor->call(this, this);\
	}

#	define OBJECT_IMPLEMENT(Class)\
	OBJECT_IMPLEMENT_FACTORY(Class)\
	OBJECT_IMPLEMENT_VISITOR(Class)



