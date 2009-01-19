#	include "ResourceDeclare.h"
#	include "ResourceVisitor.h"

//#	include "Factory.h"

#	define RESOURCE_IMPLEMENT(C)\
	Factory<String, ResourceReference *, ResourceFactoryParam>::PoolManager<C> C::ms_poolManager( &C::genObjectNew, &C::genObjectPlacement );\
ResourceReference * C::genObjectNew(const ResourceFactoryParam & _struct){\
printf("genObjectNew ResourceReference %s", _struct.name.c_str() );\
return new C( _struct );\
}\
ResourceReference * C::genObjectPlacement( void* _mem, const ResourceFactoryParam & _struct){\
printf("genObjectPlacement ResourceReference %s", _struct.name.c_str() );\
return new (_mem) C( _struct );\
}\
void C::accept(ResourceVisitor * _visitor){_visitor->visit(this);};\

//namespace {\
//volatile static TFactoryResource::\
//Auto _classRegistration(#C,C::genObject);\
//}
