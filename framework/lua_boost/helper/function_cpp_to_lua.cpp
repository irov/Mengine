#	include "function_cpp_to_lua.h"

#	include "lua_include.h"

namespace lua_boost
{
	namespace helper
	{
		//////////////////////////////////////////////////////////////////////////
		void * get_function_ptr( struct lua_State * L )
		{
			return lua_touserdata(L, lua_upvalueindex(1));
		}
	}
}
