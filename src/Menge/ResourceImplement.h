#	include "ResourceDeclare.h"
#	include "ResourceVisitor.h"

//#	include "Factory.h"

#	define RESOURCE_IMPLEMENT(C)\
	void * C::genObject( void * _data ){\
		return new C( *(ResourceFactoryParam*)_data );\
	}\
	void C::accept( ResourceVisitor * _visitor ){ _visitor->visit(this); };\

//namespace {\
//volatile static TFactoryResource::\
//Auto _classRegistration(#C,C::genObject);\
//}
