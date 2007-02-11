#	pragma once

#	include "lua_totype.h"

#	include "member_types.h"

namespace lua_boost
{
	template<class C, class F>
	struct member_cpp_to_lua
	{
		static const size_t arity = member_types<C,F>::arity;

		typedef typename member_types<C,F>::ret_type ret;

		static int callback( lua_State *L, C * obj, F member )
		{
			return callback_ret<ret>( L, obj, member );
		}


		template<class R>
		static int callback_ret( lua_State *L, C * obj, F member )
		{
			R r = func_call<arity>( L, obj, member );
			lua_pushtype( L, r );
			return 1; 
		}


		template<>
		static int callback_ret<void>( lua_State *L, C * obj, F member )
		{
			func_call<arity>( L, obj, member );
			return 0; 
		}

		template< size_t I> static ret func_call( lua_State *L, C * obj, F f );

		template<> static ret func_call<0>( lua_State *L, C * obj, F f )
		{
			return (obj->*f)();
		}

		template<> static ret func_call<1>( lua_State *L, C * obj, F f )
		{
			typedef typename member_types<C,F>::param1 _1;

			return (obj->*f)
				(lua_totype<_1>(L,1)
				);
		}

		template<> static ret func_call<2>( lua_State *L, C * obj, F f )
		{
			typedef typename member_types<C,F>::param1 _1;
			typedef typename member_types<C,F>::param2 _2;

			return (obj->*f)
				(lua_totype<_1>(L,1)
				,lua_totype<_2>(L,2)
				);
		}

		template<> static ret func_call<3>( lua_State *L, C * obj, F f )
		{
			typedef typename member_types<C,F>::param1 _1;
			typedef typename member_types<C,F>::param2 _2;
			typedef typename member_types<C,F>::param3 _3;

			return (obj->*f)
				(lua_totype<_1>(L,1)
				,lua_totype<_2>(L,2)
				,lua_totype<_3>(L,3)
				);
		}

		template<> static ret func_call<4>( lua_State *L, C * obj, F f )
		{
			typedef typename member_types<C,F>::param1 _1;
			typedef typename member_types<C,F>::param2 _2;
			typedef typename member_types<C,F>::param3 _3;
			typedef typename member_types<C,F>::param3 _4;

			return (obj->*f)
				(lua_totype<_1>(L,1)
				,lua_totype<_2>(L,2)
				,lua_totype<_3>(L,3)
				,lua_totype<_4>(L,4)
				);
		}
	};
}