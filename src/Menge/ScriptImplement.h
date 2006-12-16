#	pragma once

#	include "BaseScriptFunctionRegistration.h"

#	include "luabind/luabind.hpp"

#	define DEF_SCRIPT_FUNCTION( func, Ret, Atr )\
	Ret func Atr;\
	static class ScriptRegistration##func\
	: BaseScriptFunctionRegistration{\
		void _register( lua_State * _luaState )\
		{\
			luabind::module(_luaState) [ luabind::def( #func , &func ) ];\
		}\
	} _script_##func;\
	Ret func Atr

