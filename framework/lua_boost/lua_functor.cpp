#	include "lua_functor.h"

#	include "lua_include.h"



namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_functor::lua_functor( lua_State * _state, const char * _name )
		: lua_functor_base(_state)
		, m_name( _name )
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor::lua_functor( const lua_functor & _func )
		: lua_functor_base( _func )
		, m_name( _func.m_name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits lua_functor::call() const
	{
		lua_pushstring( m_state, m_name.c_str() );
		lua_gettable( m_state, LUA_GLOBALSINDEX );

		return lua_functor_traits(*this);
	}

	lua_functor_traits_safe lua_functor::call_safe() const
	{
		lua_pushstring( m_state, m_name.c_str() );
		lua_gettable( m_state, LUA_GLOBALSINDEX );

		bool valid = lua_isnil( m_state, -1 );

		if( valid == true )
		{
			lua_pop( m_state, 1 );
			return lua_functor_traits_safe( *this, false );
		}

		return lua_functor_traits_safe( *this, true );
	}
}