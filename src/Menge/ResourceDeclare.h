#	pragma once

#	include <string>

struct ResourceFactoryParam
{
	std::string name;
};

#	define RESOURCE_DECLARE(C)\
	public:\
	static Resource * genObject(const ResourceFactoryParam &);\
	private: