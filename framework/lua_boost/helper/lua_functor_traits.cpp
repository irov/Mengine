#	include "lua_functor_traits.h"

#	include "../lua_include.h"

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_functor_traits::lua_functor_traits( const lua_functor_base & _base )
		: lua_functor_base( _base )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void lua_functor_traits::call(int numresult)
	{
		lua_call(m_state, m_args, numresult);
		m_args = 0;
	}
}