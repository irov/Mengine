#	pragma once

#	include "Config/Typedef.h"
#	include "Factory.h"
//#	include <string>

struct ResourceFactoryParam
{
	Menge::String name;
	Menge::String category;
	Menge::String group;
};

/*namespace Menge
{
	class ResourceVisitor;
};*/

#	define RESOURCE_DECLARE(C)\
	public:\
	static ResourceReference * genObjectNew( const ResourceFactoryParam &);\
	static ResourceReference * genObjectPlacement( void*, const ResourceFactoryParam &);\
	static Factory<String, ResourceReference *, ResourceFactoryParam>::PoolManager<C> ms_poolManager;\
	void C::accept(ResourceVisitor * _visitor) override;\
	private:
//	static ResourceReference * genObject(const ResourceFactoryParam &);

//	
