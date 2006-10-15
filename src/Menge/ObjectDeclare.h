#	pragma once

#	include <string>

namespace Menge
{
	namespace Visitor
	{
		class Base;
	}
}

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
	virtual void visit(Visitor::Base *visitor);\
	static Node * genObject(const std::string &type);\
	private: