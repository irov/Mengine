#	pragma once

//#	include <string>
#	include "Config/Typedef.h"

struct FactoryGenStruct
{
	Menge::String type;
};

#	define OBJECT_DECLARE(C)\
	public:\
	static Node * genObject(const FactoryGenStruct &);\
	//private: