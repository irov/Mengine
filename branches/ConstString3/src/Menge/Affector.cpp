/*
 *	Affector.cpp
 *
 *	Created by _Berserk_ on 2.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Affector.h"

#	include "EventManager.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Affector::Affector( PyObject* _cb, EAffectorType _type )
		: m_cb(_cb)
		, m_type(_type)	
	{
		pybind::incref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	Affector::~Affector()
	{
		pybind::decref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::setId( std::size_t _id )
	{
		m_id = _id;
	}
	//////////////////////////////////////////////////////////////////////////
	EAffectorType Affector::getType() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::call( Scriptable * _scriptable, bool _isEnd )
	{
		if( m_cb == Py_None )
		{
			return;
		}
	
		PyObject * args = pybind::build_value( "(OiO)", _scriptable->getEmbed(), m_id, pybind::ret_bool(_isEnd) );

		EventManager::get()
			->addEvent( EVENT_AFFECTOR, m_cb, args );

		pybind::decref( args );
	}
}	// namespace Menge
