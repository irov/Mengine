#	pragma once

#	include "pybind/types.hpp"

namespace pybind
{
	class wrap_base
	{
	public:
		wrap_base();

	public:
		void callMethod( const char * _method, const char * _format, ... );

	protected:
		PyObject * m_pyImpl;
	};
}

