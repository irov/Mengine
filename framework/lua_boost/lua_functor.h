#	pragma once

#	include "helper/lua_functor_base.h"
#	include "helper/lua_functor_traits.h"
#	include "helper/lua_functor_traits_safe.h"

#	include "utility/lua_string.h"

struct lua_State;

namespace lua_boost
{
	class lua_functor
		: public lua_functor_base
	{
	public:
		lua_functor( lua_State * _state , const char * _name );
		lua_functor( const lua_functor &);

	public:
		lua_functor_traits call() const;
		lua_functor_traits_safe call_safe() const;

	private:
		lua_string m_name;
	};
}