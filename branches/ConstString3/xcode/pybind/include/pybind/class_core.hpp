#	pragma once

#	include "pybind/constructor.hpp"
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

namespace pybind
{
	class class_type_scope;

	class convert_adapter_interface;
	class method_adapter_interface;
	class member_adapter_interface;
	class repr_adapter_interface;

	template<class T>
	const std::type_info & class_info()
	{
		return typeid( T );
	}

	class PYBIND_API class_core
	{
	public:
		static class_type_scope * create_new_type_scope( 
			const std::type_info & _info,
			const char * _name
			);

		static void setup_new_type_scope( 
			class_type_scope * _scope,
			PyObject * _module,
			pybind::pybind_new _pynew,
			pybind::pybind_destructor _pydestructor
			);


		static void finalize();

		static PyObject * create_holder( const std::type_info & _info, void * _impl );
		static PyObject * create_impl( const std::type_info & _info, void * _impl );
		static void wrap_holder( PyObject * _obj, void * _impl );

		static void def_init( class_type_scope * _scope, constructor * _ctr );
		static void * construct( class_type_scope * _scope, PyObject * _args );

		static void set_module( class_type_scope * _scope, PyObject * _module );

		static void def_convert(
			convert_adapter_interface * _iadapter,
			const std::type_info & _info );

		static void def_method( 
			const char * _name, 
			method_adapter_interface * _iadapter, 
			int _arity, 
			const std::type_info & _info );

		static void def_member(
			const char * _name,
			member_adapter_interface * _iadapter,
			const std::type_info & _info );

		static void def_repr(
			repr_adapter_interface * _iadapter,
			const std::type_info & _info );

		static void def_getattro(
			method_adapter_interface * _iadapter,
			const std::type_info & _info );

		static void def_mapping(
			method_adapter_interface * _iadapter,
			const std::type_info & _info );

		static void def_sequence( 
			method_adapter_interface * _iadapter, 
			const std::type_info & _info );

		static void add_base_to_scope( class_type_scope * _scope, const char * _name, const std::type_info & _base, pybind_metacast cast );

		template<class B>
		static void add_base( class_type_scope * _scope, pybind_metacast cast)
		{
			const std::type_info & tptrinfo = class_info<B*>();
			const char * name = tptrinfo.name();

			const std::type_info & tinfo = class_info<B>();

			add_base_to_scope( _scope, name, tinfo, cast );			
		}

		static void * meta_cast( void * _impl, class_type_scope * _scope, const char * _name );
	};

	template<class T>
	PyObject * class_holder( T * _value )
	{
		PyObject * result = class_core::create_holder( typeid(T), _value );

		return result;
	}
}

