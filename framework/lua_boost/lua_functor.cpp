#	include "lua_functor.h"

#	include "lua_include.h"

#	include <string.h>

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_functor::lua_functor( lua_State * _state, const char * _name )
		: lua_functor_base(_state)
	{	
		if ( _name != 0 )
		{
			size_t sz = strlen(_name);
			m_name = new char[ sz + 1 ];
			strcpy_s( m_name, sz + 1, _name );
		}
		else
		{
			m_name = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor::lua_functor( const lua_functor & _func )
		: lua_functor_base( _func )
	{
		if ( _func.m_name != 0 )
		{
			size_t sz = strlen( _func.m_name );
			m_name = new char[ sz + 1 ];
			strcpy_s( m_name, sz + 1, _func.m_name );
		}
		else
		{
			m_name = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor::~lua_functor()
	{
		delete [] m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits lua_functor::call() const
	{
		lua_pushstring( m_state, m_name );
		lua_gettable( m_state, LUA_GLOBALSINDEX );

		return lua_functor_traits(*this);
	}
}