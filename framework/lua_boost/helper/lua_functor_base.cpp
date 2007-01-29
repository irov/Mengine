#	include "lua_functor_base.h"

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_functor_base::lua_functor_base( lua_State * _state )
		: m_state(_state)
		, m_args(0)
	{

	}
}