#	include "PythonCollisionObject.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionObject::PythonCollisionObject()
		: m_user(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionObject::~PythonCollisionObject()
	{
		pybind::decref( m_user );
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionObject::setPythonUser( PyObject * _user )
	{
		pybind::decref( m_user );
		m_user = _user;
		pybind::incref( m_user );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * PythonCollisionObject::getPythonUser() const
	{
		pybind::incref( m_user );
		return m_user;
	}
}