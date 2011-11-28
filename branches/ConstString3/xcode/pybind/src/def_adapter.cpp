#	include "pybind/def_adapter.hpp"
#	include "pybind/def_type.hpp"

namespace pybind
{
	namespace detail
	{
		pybind_callback extract_from_py_function( PyObject * _self )
		{
			py_function_type* func = (py_function_type*)_self;

			return func->f;
		}

		const char * name_from_py_function( PyObject * _self )
		{
			py_function_type* func = (py_function_type*)_self;

			return func->name;			
		}
	}
}
