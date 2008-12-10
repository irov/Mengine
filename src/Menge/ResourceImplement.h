#	include "ResourceDeclare.h"

//#	include "Factory.h"

#	define RESOURCE_IMPLEMENT(C)\
	Factory<String, ResourceReference *, ResourceFactoryParam>::PoolManager<C> C::ms_poolManager( &C::genObjectNew, &C::genObjectPlacement );\
ResourceReference * C::genObjectNew(const ResourceFactoryParam & _struct){\
return new C( _struct );\
}\
ResourceReference * C::genObjectPlacement( void* _mem, const ResourceFactoryParam & _struct){\
return new (_mem) C( _struct );\
}

//namespace {\
//volatile static TFactoryResource::\
//Auto _classRegistration(#C,C::genObject);\
//}