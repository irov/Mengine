#	pragma once

#	include "Config/Typedef.h"
#	include "Factory.h"

struct ResourceFactoryParam
{
	Menge::String name;
	Menge::String category;
	Menge::String group;
	Menge::String file;
};

#	define RESOURCE_DECLARE(C)\
	public:\
	static void * genObject( void * );\
	void accept( ResourceVisitor * _visitor ) override;\
	private:
//	static ResourceReference * genObject(const ResourceFactoryParam &);

//	
