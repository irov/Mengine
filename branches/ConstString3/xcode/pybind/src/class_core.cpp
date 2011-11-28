#	include "pybind/class_core.hpp"
#	include "pybind/class_type.hpp"

#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::list<class_type_scope *> TListClassType;
	static TListClassType s_listClassType;
	//////////////////////////////////////////////////////////////////////////
	void class_core::finalize()
	{
		for( TListClassType::iterator
			it = s_listClassType.begin(),
			it_end = s_listClassType.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		s_listClassType.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope * class_core::create_new_type_scope( 
		const std::type_info & _info,
		const char * _name )
	{
		const char * type_name = _info.name();
		class_type_scope * scope = new class_type_scope( _name, type_name );

		detail::reg_class_type_scope( _info, scope );
		s_listClassType.push_back( scope );

		return scope;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::setup_new_type_scope( 
		class_type_scope * _scope,
		PyObject * _module,
		pybind_new _pynew,
		pybind_destructor _pydestructor )
	{
		_scope->setup( _module, _pynew, _pydestructor );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_core::create_holder( const std::type_info & _info, void * _impl )
	{
		class_type_scope * t_scope = detail::get_class_type_scope( _info );

		return t_scope->create_holder( _impl );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_core::create_impl( const std::type_info & _info, void * _impl )
	{
		class_type_scope * t_scope = detail::get_class_type_scope( _info );

		return t_scope->create_impl( _impl );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::wrap_holder( PyObject * _obj, void * _impl )
	{
		detail::wrap( _obj, _impl, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_init( class_type_scope * _scope, constructor * _ctr )
	{
		_scope->def_init( _ctr );
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_core::construct( class_type_scope * _scope, PyObject * _args )
	{
		return _scope->construct( _args );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::set_module( class_type_scope * _scope, PyObject * _module )
	{
		_scope->set_module( _module );
	}
	void class_core::def_convert( convert_adapter_interface * _iadapter, const std::type_info & _info )
	{
		class_type_scope * scope = detail::get_class_type_scope( _info );

		scope->add_convert( _iadapter );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_method( const char * _name, method_adapter_interface * _iadapter, int _arity, const std::type_info & _info )
	{
		class_type_scope * scope = detail::get_class_type_scope( _info );

		scope->add_method( _name, _iadapter, _arity );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_member( const char * _name, member_adapter_interface * _iadapter, const std::type_info & _info )
	{
		class_type_scope * scope = detail::get_class_type_scope( _info );

		scope->add_member( _name, _iadapter );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_repr( repr_adapter_interface * _iadapter, const std::type_info & _info )
	{
		class_type_scope * scope = detail::get_class_type_scope( _info );

		scope->add_repr( _iadapter );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_getattro( method_adapter_interface * _iadapter,	const std::type_info & _info )
	{
		class_type_scope * scope = detail::get_class_type_scope( _info );

		scope->add_getattro( _iadapter );		
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_mapping( method_adapter_interface * _iadapter, const std::type_info & _info )
	{
		class_type_scope * scope = detail::get_class_type_scope( _info );

		scope->add_mapping( _iadapter );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_sequence( method_adapter_interface * _iadapter, const std::type_info & _info )
	{
		class_type_scope * scope = detail::get_class_type_scope( _info );

		scope->add_sequence( _iadapter );
	}	
	//////////////////////////////////////////////////////////////////////////
	void class_core::add_base_to_scope( class_type_scope * _scope, const char * _name, const std::type_info & _base, pybind_metacast cast )
	{
		class_type_scope * basescope = detail::get_class_type_scope( _base );

		_scope->add_base( _name, basescope, cast );
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_core::meta_cast( void * _impl, class_type_scope * _scope, const char * _name )
	{
		return _scope->metacast( _name, _impl );
	}
}
