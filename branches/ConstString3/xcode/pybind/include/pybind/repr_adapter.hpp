#	pragma once

#	include "pybind/class_core.hpp"

namespace pybind
{
	class repr_adapter_interface
		: public adapter_interface
	{
	public:
		virtual PyObject * repr( PyObject * _obj, void * _self, class_type_scope * _scope ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class repr_adapter
		: public repr_adapter_interface
	{
	public:
		repr_adapter( F _repr )
			: m_repr(_repr)
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * repr( PyObject * _obj, void * _self, class_type_scope * _scope ) override
		{
			C * inst = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			std::string repr = (*m_repr)( _obj, inst );

			return pybind::ptr( repr );
		}

	protected:
		F m_repr;

		const char * m_class_name;
		const char * m_scope_name;
	};
}

