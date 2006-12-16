#	pragma once

#	include <list>

struct lua_State;

namespace Menge
{
	class BaseScriptFunctionRegistration
	{
	public:
		BaseScriptFunctionRegistration();

	public:
		static void registration( lua_State * _luaState );

	public:
		virtual void _register( lua_State * _luaState ) = 0;
	};
}