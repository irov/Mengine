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

namespace Menge
{
	class Visitor;
}


#	define OBJECT_DECLARE(C)\
	public:\
	static Node * genObject(const std::string &);\
	virtual void visit( Visitor * );\
	private: