#	include "PythonCollisionRadar.h"
#	include "PythonCollisionObject.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionRadar::PythonCollisionRadar()
		: m_pyEnter(nullptr)
		, m_pyLeave(nullptr)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionRadar::~PythonCollisionRadar()
	{
		pybind::decref( m_pyEnter );
		pybind::decref( m_pyLeave );
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionRadar::setCallback( PyObject * _pyEnter, PyObject * _pyLeave )
	{
		pybind::decref( m_pyEnter );
		m_pyEnter = _pyEnter;
		pybind::incref( m_pyEnter );

		pybind::decref( m_pyLeave );
		m_pyLeave = _pyLeave;
		pybind::incref( m_pyLeave );
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionRadar::onRadarObjectEnter( CollisionObject * _object )
	{
		PythonCollisionObject * py_object = static_cast<PythonCollisionObject *>(_object);
		PyObject * py_user = py_object->getPythonUser();
		pybind::call( m_pyEnter, "(OOO)", pybind::ptr(this), pybind::ptr(py_object), py_user );
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionRadar::onRadarObjectLeave( CollisionObject * _object )
	{
		PythonCollisionObject * py_object = static_cast<PythonCollisionObject *>(_object);
		PyObject * py_user = py_object->getPythonUser();
		pybind::call( m_pyLeave, "(OOO)", pybind::ptr(this), pybind::ptr(py_object), py_user );
	}
}
