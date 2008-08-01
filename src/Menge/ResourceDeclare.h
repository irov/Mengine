#	pragma once

#	include "Config/Typedef.h"

//#	include <string>

struct ResourceFactoryParam
{
	Menge::String name;
	Menge::String category;
	Menge::String group;
};

#	define RESOURCE_DECLARE(C)\
	public:\
	static ResourceReference * genObject(const ResourceFactoryParam &);\
	private: