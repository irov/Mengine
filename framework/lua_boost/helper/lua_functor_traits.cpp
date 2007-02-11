#	include "lua_functor_traits.h"

#	include "lua_include.h"

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits::lua_functor_traits( const lua_functor_base & _base )
		: lua_functor_base( _base )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void lua_functor_traits::operator % ( end_call_function_format<void> )
	{
		call(0);
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_functor_traits::call(int numresult)
	{
		lua_call(m_state, m_args, numresult);
		m_args = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits::end_call_function_format<void> ret()
	{ 
		return lua_functor_traits::end_call_function_format<void>(); 
	}
}