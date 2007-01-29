#	pragma once

#	include "lua_functor.h"

#	include "helper/function_cpp_to_lua.h"

struct lua_State;

namespace lua_boost
{
	class lua_boost
	{
		typedef int (*lua_callback) (lua_State *L);

	public:
		lua_boost();
		lua_boost( lua_State * _state );

	public:
		void init();
		void init( lua_State * _state );

		void close();

		template<class F>
		void reg_function( const char * _name, F f)
		{
			reg_function_impl( 
				_name, 
				function_cpp_to_lua<F>::callback, 
				reinterpret_cast<ptrdiff_t>(f) 
				);
		}

		int do_file( const char * _file );

		int do_buffer( const char * _buff, size_t _sz );

		lua_functor functor( const char * _name );

	private:
		void reg_function_impl( const char * _name, lua_callback _func, __int64 _offset );


	private:
		lua_State * m_state;
	};
}