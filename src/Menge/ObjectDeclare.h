#	pragma once

#	include <string>

class TiXmlElement;

namespace luabind
{
	namespace adl
	{
		class object;
	}
}


#	define OBJECT_DECLARE(C)\
	public:\
	static Node * genObject(const std::string &type);\
	private: