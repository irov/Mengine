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
		void setCallback( const pybind::object & _pyFilter, const pybind::object & _pyEnter, const pybind::object & _pyLeave );
		
	public:
		PythonCollisionObject * findMinimalObject( const pybind::object & _filter ) const;

	protected:
		bool onRadarObjectFilter( CollisionObject * _object ) override;
		void onRadarObjectEnter( CollisionObject * _object ) override;
		void onRadarObjectLeave( CollisionObject * _object ) override;

	protected:
		pybind::object m_pyFilter;
		pybind::object m_pyEnter;
		pybind::object m_pyLeave;
	};	
}
