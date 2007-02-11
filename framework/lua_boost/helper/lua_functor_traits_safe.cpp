#	include "lua_functor_traits_safe.h"

#	include "lua_include.h"

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits_safe::lua_functor_traits_safe( const lua_functor_base & _base, bool _valid )
		: lua_functor_base( _base )
		, m_valid( _valid )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void lua_functor_traits_safe::operator % ( end_call_function_format<void> )
	{
		if( m_valid == true )
		{
			call(0);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_functor_traits_safe::call(int numresult)
	{
		lua_call(m_state, m_args, numresult);
		m_args = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits_safe::end_call_function_format<void> ret_safe()
	{ 
		return lua_functor_traits_safe::end_call_function_format<void>(); 
	}
}