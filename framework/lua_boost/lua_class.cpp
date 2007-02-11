#	include "lua_class.h"

#	include "lua_include.h"

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_class_impl::lua_class_impl(lua_State * L, const char * _name,
		lua_callback new_class,
		lua_callback gc_class,
		lua_callback to_string
		)
		: m_state( L )
		, m_name( _name )
	{		
		lua_newtable(L);
		m_methods = lua_gettop(L);

		luaL_newmetatable(L, _name );
		int metatable = lua_gettop(L);

		// store method table in globals so that
		// scripts can add functions written in Lua.
		lua_pushstring(L, _name );
		lua_pushvalue(L, m_methods);
		lua_settable(L, LUA_GLOBALSINDEX);

		lua_pushliteral(L, "__metatable");
		lua_pushvalue(L, m_methods);
		lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

		lua_pushliteral(L, "__index");
		lua_pushvalue(L, m_methods);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__tostring");
		lua_pushcfunction(L, to_string);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, gc_class);
		lua_settable(L, metatable);

		lua_newtable(L);                // mt for method table
		int mt = lua_gettop(L);
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, new_class);
		lua_pushliteral(L, "new");
		lua_pushvalue(L, -2);           // dup new_T function
		lua_settable(L, m_methods);       // add new_T to method table
		lua_settable(L, mt);            // mt.__call = new_T
		lua_setmetatable(L, m_methods);
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_class_impl::def_impl( const char * _name, lua_callback _thunk, MemberType * _member )
	{
		lua_pushstring(m_state, _name);
		lua_pushlightuserdata(m_state, (void*) _member );
		lua_pushcclosure(m_state, _thunk, 1);
		lua_settable(m_state, m_methods);
		lua_pop(m_state, 2);  // drop metatable and method table
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_class_impl::end_reg() const
	{
		lua_pop(m_state, 2);  // drop metatable and method table
	}
	//////////////////////////////////////////////////////////////////////////
	void * lua_class_impl::check_class( lua_State *L, int narg, const char * _name )
	{
		void *ud = luaL_checkudata(L, narg, _name );
		if(!ud) luaL_typerror(L, narg, _name );
		return ud;  // pointer to T object
	}
	//////////////////////////////////////////////////////////////////////////
	lua_class_impl::MemberType * lua_class_impl::check_member( lua_State *L, const char * _name ) 
	{
		// stack has userdata, followed by method args
		lua_remove(L, 1);  // remove self so member function args start at index 1
		// get member function from upvalue
		MemberType *member = static_cast<MemberType *>( lua_touserdata(L, lua_upvalueindex(1)) );

		return member;
	}
	//////////////////////////////////////////////////////////////////////////
	int lua_class_impl::new_class( lua_State *L, void * obj, const char * _name )
	{
		lua_remove(L, 1);   // use classname:new(), instead of classname.new()

		UserdataType *ud =
			static_cast<UserdataType*>(lua_newuserdata(L, sizeof(UserdataType)));

		ud->pObject = obj;  // store pointer to object in userdata

		luaL_getmetatable(L, _name);  // lookup metatable in Lua registry
		lua_setmetatable(L, -2);
		return 1;  // userdata containing pointer to T object
	}
	//////////////////////////////////////////////////////////////////////////
	void * lua_class_impl::gc_class( lua_State *L ) 
	{
		UserdataType *ud = static_cast<UserdataType*>(lua_touserdata(L, 1));

		void *obj = ud->pObject;

		return obj;
	}
	//////////////////////////////////////////////////////////////////////////
	int lua_class_impl::tostring_class( lua_State *L, const char * _name ) 
	{
		UserdataType *ud = static_cast<UserdataType*>(lua_touserdata(L, 1));

		lua_pushfstring(L, "%s (%p)", _name, ud->pObject);

		return 1;
	}


}