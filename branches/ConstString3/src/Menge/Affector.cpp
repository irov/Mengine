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
	Affector::Affector()
		: m_cb(0)
		, m_type(ETA_POSITION)
	{
	}
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
	void Affector::setId( size_t _id )
	{
		m_id = _id;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::setType( EAffectorType _type )
	{
		m_type = _type;
	}
	//////////////////////////////////////////////////////////////////////////
	EAffectorType Affector::getType() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::setCb( PyObject * _cb )
	{
		m_cb = _cb;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Affector::getCb() const
	{
		return m_cb;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::call( Scriptable * _scriptable, bool _isEnd )
	{
		if(  m_cb == 0 )
		{
			return;
		}

		if( pybind::is_none(m_cb) == true )
		{
			return;
		}
	
		EventManager::get()
			->addEventFormat( EVENT_AFFECTOR_END, m_cb, "(OiO)", _scriptable->getEmbed(), m_id, pybind::get_bool(_isEnd) );
	}
}	// namespace Menge
