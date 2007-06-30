#	pragma once

#	include "boost/python/object_core.hpp"

namespace Menge
{
	class ScriptObject
		: public boost::python::object
	{
	public:
		ScriptObject();
		ScriptObject( const boost::python::object & obj );

		void incref();
		void decref();
	};
}