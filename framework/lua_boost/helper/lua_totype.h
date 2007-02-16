#	pragma once

struct lua_State;

namespace lua_boost
{
	namespace lua_totype_case
	{
		bool totype( lua_State *L, int arg, bool );
		int totype( lua_State *L, int arg, int );
		size_t totype( lua_State *L, int arg, size_t );
		float totype( lua_State *L, int arg, float );
		const char * totype( lua_State *L, int arg, const char * );

		void pushtype( lua_State *L, bool b );
		void pushtype( lua_State *L, int i );
		void pushtype( lua_State *L, size_t u );
		void pushtype( lua_State *L, float f );
		void pushtype( lua_State *L, const char * s );
	}

	template<class T>
	T lua_totype( lua_State *L, int arg )
	{
		return lua_totype_case::totype( L, arg, T() );
	}

	template<class T>
	void lua_pushtype( lua_State *L, T t )
	{
		lua_totype_case::pushtype( L, t );
	}
}