#	pragma once

#	include "pybind/def_call.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API pybind_callback extract_from_py_function( PyObject * _self );
		PYBIND_API const char * name_from_py_function( PyObject * _self );
	}

	template<class F>
	class def_adapter
	{
	public:
		static PyObject * method0( PyObject * _self )
		{
			return method1( _self, 0 );
		}

		static PyObject * method1( PyObject * _self, PyObject * _args )
		{
			pybind_callback f = detail::extract_from_py_function( _self );
			const char * name = detail::name_from_py_function( _self );

			PyObject * ret = def_call<F>::call( name, (F)f, _args );

			return ret;
		}

		static PyObject * method_native( PyObject * _self, PyObject * _args, PyObject * _kwds )
		{
			pybind_callback f = detail::extract_from_py_function( _self );

			F f_adapt = (F)f;

			PyObject * ret = f_adapt(_args, _kwds);

			return ret;
		}
	};
}

