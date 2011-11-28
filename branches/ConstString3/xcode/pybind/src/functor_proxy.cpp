#	include "pybind/functor_proxy.hpp"
#	include "pybind/functor_type.hpp"

namespace pybind
{
	namespace detail
	{
		functor_proxy_interface * extract_from_py_functor( PyObject * _self )
		{
			py_functor_type * functor = (py_functor_type*)_self;

			return functor->proxy;
		}
	}
}
