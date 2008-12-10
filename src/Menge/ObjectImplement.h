#	pragma once

#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR( Class )\
	Factory<String, Node *, FactoryGenStruct>::PoolManager<Class> Class::ms_poolManager( &Class::genObjectNew, &Class::genObjectPlacement );\
	Node * Class::genObjectPlacement( void* _mem, const FactoryGenStruct & _struct){\
	Node * node = new (_mem) Class;\
	node->setType( #Class );\
	return node;\
	}\
	Node * Class::genObjectNew( const FactoryGenStruct & _struct){\
	Node * node = new Class;\
	node->setType( #Class );\
	return node;\
	}

#	else
#	define OBJECT_IMPLEMENT_GENERATOR(Class)
#	endif

#	define OBJECT_IMPLEMENT_FACTORY( Class )\
	OBJECT_IMPLEMENT_GENERATOR( Class )

#	define OBJECT_IMPLEMENT(Class)\
	OBJECT_IMPLEMENT_FACTORY(Class)



