#	include "PythonCollisionRadar.h"
#	include "PythonCollisionObject.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionRadar::PythonCollisionRadar()
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionRadar::~PythonCollisionRadar()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionRadar::setCallback( const pybind::object & _pyFilter, const pybind::object & _pyEnter, const pybind::object & _pyLeave )
	{		
		m_pyFilter = _pyFilter;
		m_pyEnter = _pyEnter;
		m_pyLeave = _pyLeave;
	}
	//////////////////////////////////////////////////////////////////////////
	PythonCollisionObject * PythonCollisionRadar::findMinimalObject( const pybind::object & _filter ) const
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

			
			if( _filter.call( obj ) == false )
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
		const pybind::object & py_user = py_object->getPythonUser();
		
		bool result = m_pyFilter.call( this, py_object, py_user );
				
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionRadar::onRadarObjectEnter( CollisionObject * _object )
	{
		PythonCollisionObject * py_object = static_cast<PythonCollisionObject *>(_object);
		const pybind::object & py_user = py_object->getPythonUser();

		m_pyEnter.call( this, py_object, py_user );
	}
	//////////////////////////////////////////////////////////////////////////
	void PythonCollisionRadar::onRadarObjectLeave( CollisionObject * _object )
	{
		PythonCollisionObject * py_object = static_cast<PythonCollisionObject *>(_object);
		const pybind::object & py_user = py_object->getPythonUser();

		m_pyLeave.call( this, py_object, py_user );
	}
}
