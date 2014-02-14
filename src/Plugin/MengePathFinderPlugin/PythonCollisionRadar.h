#	pragma once

#	include "CollisionRadar.h"

#	include "PythonCollisionObject.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	class PythonCollisionRadar
		: public CollisionRadar
	{
	public:
		PythonCollisionRadar();
		~PythonCollisionRadar();

	public:
		void setCallback( PyObject * _pyFilter, PyObject * _pyEnter, PyObject * _pyLeave );
		
	public:
		PythonCollisionObject * findMinimalObject( PyObject * _filter ) const;

	protected:
		bool onRadarObjectFilter( CollisionObject * _object ) override;
		void onRadarObjectEnter( CollisionObject * _object ) override;
		void onRadarObjectLeave( CollisionObject * _object ) override;

	protected:
		PyObject * m_pyFilter;
		PyObject * m_pyEnter;
		PyObject * m_pyLeave;
	};	
}
