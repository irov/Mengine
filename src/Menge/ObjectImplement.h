#	pragma once

#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR( Class )\
	Node * Class::genObject(const FactoryGenStruct & _struct){\
	Node * node = new Class;\
	node->setType( MENGE_TEXT(#Class) );\
	return node;\
	}
#	else
#	define OBJECT_IMPLEMENT_GENERATOR(Class)
#	endif

#	define OBJECT_IMPLEMENT_FACTORY( Class )\
	OBJECT_IMPLEMENT_GENERATOR( Class )

#	define OBJECT_IMPLEMENT(Class)\
	OBJECT_IMPLEMENT_FACTORY(Class)



