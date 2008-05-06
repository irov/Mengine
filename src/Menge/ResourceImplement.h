#	include "ResourceDeclare.h"

//#	include "Factory.h"

#	define RESOURCE_IMPLEMENT(C)\
ResourceReference * C::genObject(const ResourceFactoryParam & _struct){\
return new C( _struct );\
}
//namespace {\
//volatile static TFactoryResource::\
//Auto _classRegistration(#C,C::genObject);\
//}