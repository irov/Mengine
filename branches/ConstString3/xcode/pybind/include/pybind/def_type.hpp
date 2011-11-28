#	pragma once

#	include "pybind/types.hpp"
#	include "config/python.hpp"

namespace pybind
{
	struct py_function_type{
		PyObject_HEAD
		pybind_callback f;
		const char * name;
	};
}

