#	pragma once

#	include "lua_functor_base.h"
#	include "lua_safe_result.h"

#	include "lua_totype.h"

namespace lua_boost
{
	class lua_functor_traits_safe
		: public lua_functor_base
	{
	public:
	public:
		lua_functor_traits_safe( const lua_functor_base & base , bool valid );

	public:
		template<class T>
		lua_functor_traits_safe & operator % ( T value )
		{ 
			if( m_valid )
			{
				lua_pushtype( m_state, value ); 
				++m_args;
			}

			return *this; 
		}

		template<class T>
		struct end_call_function_format{};

		template<class R>
		safe_result<R> operator % ( end_call_function_format<R> )
		{
			if( m_valid == true )
			{
				call(1);
				return safe_result<R>( lua_totype<R>( m_state, -1 ) , true );
			}
			else
			{
				return safe_result<R>( R() , false );
			}
		}

		void operator % ( end_call_function_format<void> );

	private:
		void call( int numresult );

	private:
		bool m_valid;
	};

	template<class T>
	lua_functor_traits_safe::end_call_function_format<T> ret_safe()
	{ 
		return lua_functor_traits_safe::end_call_function_format<T>(); 
	}

	lua_functor_traits_safe::end_call_function_format<void> ret_safe();
}