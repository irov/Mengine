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
		void setPythonUser( const pybind::object & _user );
		const pybind::object & getPythonUser() const;

	protected:
		pybind::object m_user;
	};	
}
