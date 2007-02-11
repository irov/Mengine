#	pragma once

#	include "lua_totype.h"

#	include "function_types.h"

namespace lua_boost
{
	namespace helper
	{
		void * get_function_ptr( struct lua_State * L );
	}

	template<class F>
	struct function_cpp_to_lua
	{
		static const size_t arity = function_types<F>::arity;
		typedef typename function_types<F>::ret_type ret;

		static int callback( lua_State *L )
		{
			return callback_ret<ret>( L );
		}

		template<class R>
		static int callback_ret( lua_State *L )
		{
			R r = func_call<arity>(
				L, 
				reinterpret_cast<F>( helper::get_function_ptr(L) )
				);
			lua_pushtype( L, r );  
			return 1; 
		}


		template<>
		static int callback_ret<void>( lua_State *L )
		{
			func_call<arity>(
				L, 
				reinterpret_cast<F>( helper::get_function_ptr(L) )
				);
			return 0; 
		}

		template< size_t I> static ret func_call( lua_State *L, F f );

		template<> static ret func_call<0>( lua_State *L, F f )
		{
			return (*f)();
		}

		template<> static ret func_call<1>( lua_State *L, F f )
		{
			typedef typename function_types<F>::param1 _1;

			return (*f)
				(lua_totype<_1>(L,1)
				);
		}

		template<> static ret func_call<2>( lua_State *L, F f )
		{
			typedef typename function_types<F>::param1 _1;
			typedef typename function_types<F>::param2 _2;

			return (*f)
				(lua_totype<_1>(L,1)
				,lua_totype<_2>(L,2)
				);
		}

		template<> static ret func_call<3>( lua_State *L, F f )
		{
			typedef typename function_types<F>::param1 _1;
			typedef typename function_types<F>::param2 _2;
			typedef typename function_types<F>::param3 _3;

			return (*f)
				(lua_totype<_1>(L,1)
				,lua_totype<_2>(L,2)
				,lua_totype<_3>(L,3)
				);
		}

		template<> static ret func_call<4>( lua_State *L, F f )
		{
			typedef typename function_types<F>::param1 _1;
			typedef typename function_types<F>::param2 _2;
			typedef typename function_types<F>::param3 _3;
			typedef typename function_types<F>::param3 _4;

			return (*f)
				(lua_totype<_1>(L,1)
				,lua_totype<_2>(L,2)
				,lua_totype<_3>(L,3)
				,lua_totype<_4>(L,4)
				);
		}
	};
}