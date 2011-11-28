#	pragma once

#	include "pybind/bases.hpp"

#	include "pybind/class_core.hpp"
#	include "pybind/class_type.hpp"
#	include "pybind/function_parser.hpp"

#	include "pybind/method_adapter.hpp"
#	include "pybind/member_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#	include "pybind/repr_adapter.hpp"
#	include "pybind/type_cast.hpp"

#	include <list>

#	include <typeinfo>

namespace pybind
{	
	typedef bases<void,void,void,void,void,void,void> no_bases;

	template<class C, class B = no_bases>
	class base_
	{
	protected:
		typedef B bases_type;

	public:
		virtual void setup_extract() = 0;

	public:
		base_( const char * _name, pybind_new _pynew, pybind_destructor _pydestructor, PyObject * _module )
		{
			class_type_scope * scope = class_core::create_new_type_scope( class_info<C>(), _name);

			setup_bases( scope );

			class_core::setup_new_type_scope( 
				scope, 
				_module, 
				_pynew, 
				_pydestructor 
				);
		}

		template<class BB>
		static void * meta_cast( void * _ptr )
		{
			return static_cast<BB*>(static_cast<C*>(_ptr));
		}

		void setup_bases( class_type_scope * _scope )
		{
			int arity = B::base_arity;

			if( arity-- > 0 )class_core::add_base<typename B::base0>(_scope, &meta_cast<typename B::base0>);
			if( arity-- > 0 )class_core::add_base<typename B::base1>(_scope, &meta_cast<typename B::base1>);
			if( arity-- > 0 )class_core::add_base<typename B::base2>(_scope, &meta_cast<typename B::base2>);
			if( arity-- > 0 )class_core::add_base<typename B::base3>(_scope, &meta_cast<typename B::base3>);
			if( arity-- > 0 )class_core::add_base<typename B::base4>(_scope, &meta_cast<typename B::base4>);
			if( arity-- > 0 )class_core::add_base<typename B::base5>(_scope, &meta_cast<typename B::base5>);
			if( arity-- > 0 )class_core::add_base<typename B::base6>(_scope, &meta_cast<typename B::base6>);
			if( arity-- > 0 )class_core::add_base<typename B::base7>(_scope, &meta_cast<typename B::base7>);
			if( arity-- > 0 )class_core::add_base<typename B::base8>(_scope, &meta_cast<typename B::base8>);
		}

		template<class C0, class C1, class C2, class C3, class C4, class C5>
		base_ & def( const init<C0,C1,C2,C3,C4,C5> & _init )
		{
			constructor * ctr = 
				new constructor_params<C, init<C0,C1,C2,C3,C4,C5> >();

			class_core::def_init( scope(), ctr );

			return *this;
		}

		template<class F>
		base_ & def( const char * _name, F f )
		{			
			method_adapter_interface * iadapter =
				new method_adapter<C, F>(f, _name);

			typedef typename function_parser<F>::result t_info;

			class_core::def_method(
				_name,
				iadapter,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		//base_ & def_construct( F f )

		template<class F>
		base_ & def_native( const char * _name, F f )
		{			
			method_adapter_interface * iadapter =
				new method_adapter_native<C, F>(f, _name);

			typedef typename function_parser<F>::result t_info;

			class_core::def_method(
				_name,
				iadapter,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		template<class F>
		base_ & def_convert( F f )
		{
			convert_adapter_interface * iadapter =
				new convert_adapter<F>(f);

			class_core::def_convert(
				iadapter,
				class_info<C>()
				);

			return *this;
		}

		template<class F>
		base_ & def_static( const char * _name, F f )
		{			
			method_adapter_interface * iadapter =
				new method_adapter_proxy_function<C, F>(f, _name);

			typedef typename function_parser<F>::result t_info;

			class_core::def_method(
				_name,
				iadapter,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		template<class P, class F>
		base_ & def_static( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface * iadapter =
				new method_adapter_proxy_member<C, P, F>(_proxy, f, _name);

			typedef typename function_parser<F>::result t_info;

			class_core::def_method(
				_name,
				iadapter,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		template<class A>
		base_ & def_member( const char * _name, A C:: * a )
		{
			member_adapter_interface * iadapter =
				new member_adapter<C, A>(_name, a);

			class_core::def_member(
				_name,
				iadapter,
				class_info<C>()
				);

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface * iadapter =
				new member_adapter_property<C, FG, FS>(_get, _set);

			class_core::def_member(
				_name,
				iadapter,
				class_info<C>()
				);

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property_static( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface * iadapter =
				new member_adapter_property_static<C, FG, FS>( _get, _set);

			class_core::def_member(
				_name,
				iadapter,
				class_info<C>()
				);

			return *this;
		}

		template<class F>
		base_ & def_repr( F _repr )
		{
			repr_adapter_interface * iadapter =
				new repr_adapter<C, F>( _repr );

			class_core::def_repr( 
				iadapter, 
				class_info<C>() 
				);

			return *this;
		}

		template<class F>
		base_ & def_getattro( F _fn )
		{
			method_adapter_interface * iadapter =
				new method_adapter<C, F>(_fn, "getattro");

			class_core::def_getattro( 
				iadapter, 
				class_info<C>() 
				);

			return *this;
		}

		template<class F>
		base_ & def_mapping( F _fn )
		{
			method_adapter_interface * iadapter =
				new method_adapter<C, F>(_fn, "getmap");

			class_core::def_mapping( 
				iadapter, 
				class_info<C>() 
				);

			return *this;
		}

		//template<class F>
		//base_ & def_sequence( F _fn )
		//{
		//	method_adapter_interface * iadapter =
		//		new method_adapter<C, F>(_fn, "sequence");

		//	class_core::def_sequence( 
		//		iadapter, 
		//		class_info<C>() 
		//		);

		//	return *this;
		//}

		template<class F>
		base_ & def_static_sequence( F _fn )
		{
			method_adapter_interface * iadapter =
				new method_adapter_proxy_function<C, F>(_fn, "sequence");

			class_core::def_sequence( 
				iadapter, 
				class_info<C>() 
				);

			return *this;
		}

		

		static void *
			new_interface( pybind::class_type_scope * _scope, PyObject * _args, PyObject * _kwds )
		{
			return 0;
		}


		static void *
			new_( pybind::class_type_scope * _scope, PyObject * _args, PyObject * _kwds )
		{
			void * impl = class_core::construct( scope(), _args );

			return impl;
		}

		static void 
			dealloc_only_python( void * impl )
		{
		}

		static void
			dealloc_( void * impl )
		{
			C * obj = static_cast<C*>(impl);
			delete obj;
		}

		static class_type_scope * scope()
		{
			return detail::get_class_type_scope( class_info<C>() );
		}
	};

	template<class C, bool V> 
	struct extract_class_type_ptr
		: public type_cast_result<C *, V>
	{
		bool apply( PyObject * _obj, C *& _value ) override
		{
			const std::type_info & tinfo = class_info<C>();
			const std::type_info & tptrinfo = class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				detail::error_invalid_extract( _obj, tinfo );

				_value = NULL;

				return false;
			}

			_value = static_cast<C*>(impl);

			return true;
		}

		PyObject * wrap( C * _class ) override
		{
			return class_core::create_holder( class_info<C>(), (void *)_class );
		}
	};

	template<class C, bool V>
	struct extract_class_type_ref
		: public type_cast_result<C, V>
	{
		bool apply( PyObject * _obj, C & _value ) override
		{
			const std::type_info & tinfo = class_info<C>();
			const std::type_info & tptrinfo = class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				if( detail::convert_object( _obj, tinfo, &_value ) == false )
				{
					detail::error_invalid_extract( _obj, tinfo );

					return false;
				}

				return true;
			}

			_value = *static_cast<C*>(impl);

			return true;
		}

		PyObject * wrap( const C & _class )
		{
			//return 0;			
			return class_core::create_impl( class_info<C>(), (void *)new C(_class) );
		}
	};

	template<class C, class B = no_bases>
	class class_
		: public base_<C,B>
	{
	public:
		class_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, &base_<C,B>::new_, &base_<C,B>::dealloc_, _module )
		{
			if( external_extract )
			{
				setup_extract();
			}

			constructor * empty_ctr = 
				new constructor_params<C, init<> >();

			class_core::def_init( base_<C,B>::scope(), empty_ctr );
		}

	protected:
		void setup_extract() override
		{
			static extract_class_type_ptr<C, false> s_registartor_ptr;
			static extract_class_type_ref<C, true> s_registartor_ref;
		}
	};

	template<class C, class B = no_bases>
	class proxy_
		: public base_<C,B>
	{
	protected:
		void setup_extract() override
		{
			static extract_class_type_ptr<C, false> s_registartor_ptr;
			static extract_class_type_ref<C, true> s_registartor_ref;
		}

	public:
		proxy_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, &base_<C,B>::new_, &base_<C,B>::dealloc_only_python, _module )
		{
			if( external_extract )
			{
				setup_extract();
			}

			constructor * empty_ctr = 
				new constructor_params<C, init<> >();

			class_core::def_init( base_<C,B>::scope(), empty_ctr );
		}
	};

	template<class C, class B = no_bases>
	class interface_
		: public base_<C,B>
	{
	public:
		typedef extract_class_type_ptr<C, false> extract_ptr_type;

	public:
		interface_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, &base_<C,B>::new_interface, &base_<C,B>::dealloc_only_python, _module )
		{
			if( external_extract )
			{
				setup_extract();
			}
		}

	protected:
		void setup_extract() override
		{
			static extract_ptr_type s_registartor_ptr;
		}
	};
}

