# pragma once

struct lua_State;

namespace lua_boost
{
	class lua_functor_base
	{
	public:
		lua_functor_base( lua_State * _state );

	protected:
		lua_State * m_state;

		int m_args;
	};
}