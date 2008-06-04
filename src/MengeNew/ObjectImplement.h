#	pragma once

#	include "ScriptClassWrapper.h"
#	include "NodeFactory.h"
#	include "Visitor.h"

#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR( Class )\
	Node * Class::genObject(const FactoryGenStruct & _struct){\
	Node * node = new Class;\
	node->setType( #Class );\
	return node;\
	}
#	else
#	define OBJECT_IMPLEMENT_GENERATOR(Class)
#	endif


#	define OBJECT_IMPLEMENT_FACTORY( Class )\
	OBJECT_IMPLEMENT_GENERATOR( Class )\
	void Class::visit( Visitor * _visitor ){ _visitor->procces_impl( this ); }\
	namespace {\
	volatile static NodeFactory::\
	Auto _classRegistration(#Class,&Class::genObject);\
	}

#	define OBJECT_IMPLEMENT(Class)\
	OBJECT_IMPLEMENT_FACTORY(Class)



