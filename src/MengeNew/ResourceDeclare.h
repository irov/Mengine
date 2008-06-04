#	pragma once

#	include <string>

struct ResourceFactoryParam
{
	std::string name;
	std::string category;
};

#	define RESOURCE_DECLARE(C)\
	public:\
	static ResourceReference * genObject(const ResourceFactoryParam &);\
	private: