#	pragma once

#	include "pybind/types.hpp"
#	include "config/python.hpp"
#	include "pybind/functor_proxy.hpp"

namespace pybind
{
	struct py_functor_type{
		PyObject_HEAD
		functor_proxy_interface * proxy;
	};
}

