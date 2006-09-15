#	pragma once

#	include <string>

namespace RvEngine
{
	class SceneManager;

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
	static Node * genObject(SceneManager *sceneManager, const std::string &type);\
	private: