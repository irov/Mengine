#	include "lua_boost.h"

#	include "lua_include.h"

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_boost::lua_boost()
	: m_state(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	lua_boost::lua_boost( lua_State * _state )
		: m_state(_state)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_boost::init()
	{
		m_state = lua_open();

		luaopen_base(m_state);
		luaopen_string(m_state);
		luaopen_debug(m_state);
		luaopen_io(m_state);		
		luaopen_math(m_state);	
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_boost::init( lua_State * _state )
	{
		m_state = _state;
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_boost::close()
	{
		lua_close( m_state );
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_boost::reg_function_impl( const char *_name, lua_CFunction _func, void * _offset )
	{
		lua_pushstring(m_state, _name );
		lua_pushlightuserdata(m_state, _offset );
		lua_pushcclosure(m_state, _func, 1);
		lua_settable(m_state, LUA_GLOBALSINDEX);
	}
	//////////////////////////////////////////////////////////////////////////
	int lua_boost::do_file( const char * _file )
	{
		int i_line = luaL_loadfile( m_state, _file );
		if( i_line == 0 )
		{
			lua_pcall(m_state, 0, 0, 0);
		}
		else
		{
			const char * str_error = lua_tostring(m_state, -1);
			lua_pop(m_state, 1);
		}
		return i_line;
	}
	//////////////////////////////////////////////////////////////////////////
	int lua_boost::do_buffer( const char * _buff, size_t _sz )
	{
		int i_line = luaL_loadbuffer( m_state, _buff, _sz, "lua_boost");
		if( i_line == 0 )
		{
			lua_pcall(m_state, 0, 0, 0);
		}
		else
		{
			const char * str_error = lua_tostring(m_state, -1);
			lua_pop(m_state, 1);
		}
		return i_line;
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor lua_boost::functor( const char * _name )
	{
		return lua_functor( m_state, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits lua_boost::call_function( const char * _name )
	{
		return functor(_name).call();
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits_safe lua_boost::call_function_safe( const char * _name )
	{
		return functor(_name).call_safe();
	}
	//////////////////////////////////////////////////////////////////////////
	bool lua_boost::is_nil( const char * _name )
	{
		lua_pushstring( m_state, _name );
		lua_gettable( m_state, LUA_GLOBALSINDEX );

		bool result = lua_isnil( m_state, -1 );
		lua_pop( m_state, 1 );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	lua_reg_module lua_boost::reg_module()
	{
		return lua_reg_module();
	}


}