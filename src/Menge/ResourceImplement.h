#	include "ResourceDeclare.h"

#	include "Factory.h"

namespace Menge
{
	typedef Menge::Factory< std::string, ResourceReference *, ResourceFactoryParam> TFactoryResource;
}

#	define RESOURCE_IMPLEMENT(C)\
ResourceReference * C::genObject(const ResourceFactoryParam & _struct){\
return new C( _struct.name );\
}\
namespace {\
volatile static TFactoryResource::\
Auto _classRegistration(#C,C::genObject);\
}