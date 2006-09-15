// ***************************************************************
//  ScriptObjectHelper.h   version:  1.0   ·  date: 05/10/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************


#	pragma once

#	include <string>

class CLuaScript;

namespace luabind{
	namespace adl{
		class object;
	}
}
namespace RvEngine
{
	class Node;

	namespace ScriptableCaster
	{
		typedef luabind::adl::object * (*TFunTypeCast)(Node *,CLuaScript *);

		class Auto
		{
		public:
			Auto(const std::string &_key, TFunTypeCast _func);
		};

		void registry(const std::string &_key, TFunTypeCast _func);

		luabind::adl::object * cast(size_t _typeId, Node *_node);
		luabind::adl::object * cast(const std::string & _key, Node *_node);
		luabind::adl::object * cast(Node *_node);
	};
}