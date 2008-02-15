#	pragma once

#	include <string>

struct ResourceFactoryParam
{
	std::string name;
};

#	define RESOURCE_DECLARE(C)\
	public:\
	static ResourceReference * genObject(const ResourceFactoryParam &);\
	private: