#	pragma once

#	include "CollisionObject.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	class PythonCollisionObject
		: public CollisionObject
	{
	public:
		PythonCollisionObject();
		~PythonCollisionObject();

	public:
		void setPythonUser( PyObject * _user );
		PyObject * getPythonUser() const;

	protected:
		PyObject * m_user;
	};	
}
