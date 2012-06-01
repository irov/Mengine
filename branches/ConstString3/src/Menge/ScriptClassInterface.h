#	pragma once

#	include "pybind/pybind.hpp"

namespace Menge
{
	class Node;

	class ScriptClassInterface
	{
	public:
		virtual PyObject * wrap( Node * _node ) = 0;
		
	public:
		virtual void destroy() = 0;
	};
}