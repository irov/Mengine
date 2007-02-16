#	pragma once

#	include "lua_functor.h"
#	include "lua_class.h"

#	include "helper/lua_reg_module.h"
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
			reg_function_impl( _name, function_cpp_to_lua<F>::callback, f );
		}

		int do_file( const char * _file );

		int do_buffer( const char * _buff, size_t _sz );

		bool is_nil( const char * _name );

		lua_functor functor( const char * _name );

		lua_functor_traits call_function( const char * _name );
		lua_functor_traits_safe call_function_safe( const char * _name );

		template<class T>
		void ref_object( const char * _name, T * _self )
		{
			const char * className = lua_class<T>::className();
			lua_class_impl::new_object( m_state, _self, className, _name );
		}

		template<class T>
		lua_class<T> reg_class( const char * _name )
		{
			return lua_class<T>( m_state, _name );
		}

		lua_reg_module reg_module();

	private:
		void reg_function_impl( const char * _name, lua_callback _func, void * _offset );


	private:
		lua_State * m_state;
	};
}