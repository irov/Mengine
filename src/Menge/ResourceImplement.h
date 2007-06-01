#	include "ResourceDeclare.h"

#	include "Factory.h"

namespace Menge
{
	typedef Utility::Factory< std::string, Resource *, ResourceFactoryParam> TFactoryResource;
}

#	define RESOURCE_IMPLEMENT(C)\
Resource * C::genObject(const ResourceFactoryParam & _struct){\
return new C( _struct.name );\
}\
namespace {\
volatile static TFactoryResource::\
Auto _classRegistration(#C,C::genObject);\
}