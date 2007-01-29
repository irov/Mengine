#	pragma once

#	include "lua_functor_base.h"

#	include "lua_totype.h"

namespace lua_boost
{
	class lua_functor_traits
		: public lua_functor_base
	{
	public:
		lua_functor_traits( const lua_functor_base & );

	public:
		template<class T>
		lua_functor_traits & operator % ( T value )
		{ 
			lua_pushtype( m_state, value ); 
			++m_args;
			return *this; 
		}

		template<class T>
		struct end_call_function_format{};

		template<class R>
		R operator % ( end_call_function_format<R> )
		{
			call(1);
			return lua_totype<R>( m_state, -1 );
		}

		template<>
		void operator % ( end_call_function_format<void> )
		{
			call(0);
		}

	private:
		void call( int numresult );
	};

	template<class T>
	lua_functor_traits::end_call_function_format<T> ret()
	{ 
		return lua_functor_traits::end_call_function_format<T>(); 
	}
}