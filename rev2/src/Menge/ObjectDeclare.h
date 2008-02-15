#	pragma once

#	include <string>

struct FactoryGenStruct
{
	std::string type;
};

#	define OBJECT_DECLARE(C)\
	public:\
	static Node * genObject(const FactoryGenStruct &);\
	//private: