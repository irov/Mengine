#	pragma once

#	include "helper/member_cpp_to_lua.h"

#	include "utility/lua_string.h"

struct lua_State;

namespace lua_boost
{
	class lua_class_impl
	{
	protected:
		typedef int (*lua_callback) (lua_State *L);

		struct UserdataType
		{
			void * pObject;
		};

		struct MemberType {};

	public:
		lua_class_impl( lua_State * _state, const char * _name, 
			lua_callback new_class,
			lua_callback gc_class,
			lua_callback to_string
			);

	public:
		void def_impl( const char * _name, lua_callback _thunk, MemberType * _member );
		
		static void * check_class( lua_State *L, int narg, const char * _name );
		static MemberType * check_member( lua_State *L, const char * _name);

		// create a new T object and
		// push onto the Lua stack a userdata containing a pointer to T object
		static int new_class( lua_State *L, void * obj, const char * _name );
		static void * gc_class( lua_State *L );
		static int tostring_class( lua_State *L, const char * _name );

	public:
		void end_reg() const;

	private:
		mutable lua_State *	m_state;
		lua_string	m_name;

		int m_methods;
	};

	template<class C>
	class lua_class
		: public lua_class_impl
	{
	public:

		template<class F>
		struct MemberType
			: public lua_class_impl::MemberType
		{
			MemberType( F _f ) : f( _f ) {}

			F f;
		};

	public:
		lua_class( lua_State * _state, const char * _name )
			: lua_class_impl( _state, _name, new_class, gc_class, tostring_class )
		{
			s_name.assign( _name );
		}

		template<class F>
		static int thunk( lua_State * _state )
		{
			C *obj = static_cast<C *>(
				lua_class_impl::check_class( _state, 1, s_name.c_str() )
				);
			
			MemberType<F> *member = static_cast<MemberType<F> *>( 
				lua_class_impl::check_member( _state, s_name.c_str() ) 
				);
	
			return member_cpp_to_lua<C,F>::callback( _state, obj, member->f );
		}

		template<class F>
		lua_class & def( const char * _name, F f )
		{
			def_impl( _name, thunk<F> , new MemberType<F>(f) );

			return *this;
		}

		// create a new T object and
		// push onto the Lua stack a userdata containing a pointer to T object
		static int new_class(lua_State *L) 
		{
			C *obj = new C();  // call constructor for T objects

			return lua_class_impl::new_class( L , obj, s_name.c_str() );
		}

		// garbage collection metamethod
		static int gc_class(lua_State *L) 
		{
			C * obj = static_cast<C*>( 
				lua_class_impl::gc_class(L) 
				);

			delete obj;  // call destructor for T objects
			return 0;
		}

		static int tostring_class(lua_State *L) 
		{
			return lua_class_impl::tostring_class( L, s_name.c_str() );
		}

	protected:
		static lua_string s_name; 
	};

	template< class C >
	lua_string lua_class<C>::s_name(0);
}