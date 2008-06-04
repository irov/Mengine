#	pragma once

#	include <string>

struct FactoryGenStruct
{
	std::string type;
};

#	define OBJECT_DECLARE(C)\
	public:\
	static Node * genObject(const FactoryGenStruct &);\
	void visit( Visitor * _visitor ) override;\
	//private: