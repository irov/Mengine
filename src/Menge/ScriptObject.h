#	pragma once

#	include "boost/python/object_core.hpp"

namespace Menge
{
	class ScriptObject
		: public boost::python::object
	{
	public:
		ScriptObject( const boost::python::object & obj )
			: boost::python::object( obj )
		{
		}
	};
}