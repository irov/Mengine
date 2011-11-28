#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_adapter.hpp"
#	include "pybind/class_core.hpp"

namespace pybind
{
	class member_adapter_interface
		: public adapter_interface
	{
	public:
		virtual PyObject * get( void * _self, class_type_scope * _scope ) = 0;
		virtual int set( void * _self, PyObject * _args, class_type_scope * _scope ) = 0;
	};

	template<class C, class A>
	class member_adapter
		: public member_adapter_interface
	{
	public:
		member_adapter( const char * _name, A C:: * _member )
			: m_member(_member)
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * get( void * _self, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			PyObject * py_value = pybind::ptr( obj->*m_member );

			return py_value;
		}

		int set( void * _self, PyObject * _args, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			obj->*m_member = pybind::extract<A>( _args );

			return 1;
		}

	protected:
		A C:: * m_member;

		const char * m_class_name;
		const char * m_scope_name;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FG, class FS>
	class member_adapter_property
		: public member_adapter_interface
	{
	public:
		member_adapter_property( FG _get, FS _set )
			: m_get(_get)
			, m_set(_set)
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * get( void * _self, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			PyObject * py_value = pybind::ptr( (obj->*m_get)() );

			return py_value;
		}

		int set( void * _self, PyObject * _args, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			typedef typename function_parser<FS>::result f_info;
				
			(obj->*m_set)(
				pybind::extract<typename f_info::param0>( _args )
				);

			return 1;
		}

	protected:
		FG m_get;
		FS m_set;

		const char * m_class_name;
		const char * m_scope_name;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FG, class FS>
	class member_adapter_property_static
		: public member_adapter_interface
	{
	public:
		member_adapter_property_static( FG _get, FS _set )
			: m_get(_get)
			, m_set(_set)
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * get( void * _self, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			PyObject * py_value = pybind::ptr( (*m_get)( obj ) );

			return py_value;
		}

		int set( void * _self, PyObject * _args, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			typedef typename function_parser<FS>::result f_info;

			(*m_set)( obj
				, pybind::extract<typename f_info::param1>( _args )
				);

			return 1;
		}

	protected:
		FG m_get;
		FS m_set;

		const char * m_class_name;
		const char * m_scope_name;
	};
}

