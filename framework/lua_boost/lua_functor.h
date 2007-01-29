#	pragma once

#	include "helper/lua_functor_base.h"
#	include "helper/lua_functor_traits.h"

struct lua_State;

namespace lua_boost
{
	class lua_functor
		: public lua_functor_base
	{
	public:
		lua_functor( lua_State * _state , const char * _name );
		lua_functor( const lua_functor &);

		~lua_functor();

	public:
		lua_functor_traits call() const;

	private:
		char * m_name;
	};
}