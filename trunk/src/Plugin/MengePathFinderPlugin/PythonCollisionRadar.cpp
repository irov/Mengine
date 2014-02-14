#	include "PythonCollisionRadar.h"
#	include "PythonCollisionObject.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionRadar::PythonCollisionRadar()
		: m_pyFilter(nullptr)
		, m_pyEnter(nullptr)
		, m_pyLeave(nullptr)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionRadar::~PythonCollisionRadar()
	{
		pybind::decref( m_pyFilter );
		pybind::decref( m_pyEnter );
		pybind::decref( m_pyLeave );
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionRadar::setCallback( PyObject * _pyFilter, PyObject * _pyEnter, PyObject * _pyLeave )
	{		
		pybind::decref( m_pyFilter );
		m_pyFilter = _pyFilter;
		pybind::incref( m_pyFilter );

		pybind::decref( m_pyEnter );
		m_pyEnter = _pyEnter;
		pybind::incref( m_pyEnter );

		pybind::decref( m_pyLeave );
		m_pyLeave = _pyLeave;
		pybind::incref( m_pyLeave );
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionObject * PythonCollisionRadar::findMinimalObject( PyObject * _filter ) const
	{
		PythonCollisionObject * find_obj = nullptr;
		float find_distance = m_radius;

		const mt::vec3f & radar_wp = this->getWorldPosition();

		for( TVectorCollisionObjects::const_iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			const CollisionObjectDesc & desc = *it;

			if( desc.real == false )
			{
				continue;
			}

			if( desc.object->getDead() == true )
			{
				continue;
			}

			PythonCollisionObject * obj = static_cast<PythonCollisionObject *>(desc.object);

			PyObject * py_ask = pybind::ask( _filter, "(O)", pybind::ptr(obj) );

			if( pybind::is_true( py_ask ) == false )
			{
				continue;
			}

			const mt::vec3f & wp = obj->getWorldPosition();

			float distance = mt::length_v3_v3( radar_wp, wp );

			float obj_radius = obj->getRadius();
			float test_radius = distance - obj_radius;

			if( find_distance >= test_radius )
			{
				find_distance = test_radius;
				find_obj = obj;
			}			
		}

		return find_obj;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PythonCollisionRadar::onRadarObjectFilter( CollisionObject * _object )
	{
		PythonCollisionObject * py_object = static_cast<PythonCollisionObject *>(_object);
		PyObject * py_user = py_object->getPythonUser();
		PyObject * py_ask = pybind::ask( m_pyFilter, "(OOO)", pybind::ptr(this), pybind::ptr(py_object), py_user );

		bool result = pybind::is_true( py_ask );
		
		return result;
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
