#	pragma once

#	include "pybind/member_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_member_type{
		PyObject_HEAD

		const char * name;
		member_adapter_interface * iadpter;
	};

	class member_type_scope
	{
	public:
		static PyObject * instance( const char * _name, member_adapter_interface * _iadpter  );
	};

	void initialize_members();
}

