#	pragma once

#	include "CollisionRadar.h"

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
		void setCallback( PyObject * _pyEnter, PyObject * _pyLeave );
		
	public:
		virtual void onRadarObjectEnter( CollisionObject * _object );
		virtual void onRadarObjectLeave( CollisionObject * _object );

	protected:
		PyObject * m_cb;

		PyObject * m_pyEnter;
		PyObject * m_pyLeave;
	};	
}
