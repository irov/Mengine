#	pragma once

#	include "pybind/method_adapter.hpp"

#	include "pybind/types.hpp"

namespace pybind
{
	class method_type_scope
	{
	public:
		static PyObject * instance( const char * _name, method_adapter_interface * _ifunc, PyTypeObject * _type );
	};

	void initialize_methods();
	void finalize_methods();
}

