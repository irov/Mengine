#	pragma once

#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR( Class )\
	void * Class::genObject( void * ){\
	Node * node = new Class;\
	node->setType( #Class );\
	return (void*)node;\
	}

#	else
#	define OBJECT_IMPLEMENT_GENERATOR(Class)
#	endif

#	define OBJECT_IMPLEMENT_FACTORY( Class )\
	OBJECT_IMPLEMENT_GENERATOR( Class )

#	define OBJECT_IMPLEMENT(Class)\
	OBJECT_IMPLEMENT_FACTORY(Class)



