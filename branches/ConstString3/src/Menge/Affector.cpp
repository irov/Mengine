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
	
		PyObject * args = pybind::build_value( "(Ob)", _scriptable->getEmbed(), _isEnd );

		EventManager::get()
			->addEvent( EVENT_AFFECTOR, m_cb, args );
	}
}	// namespace Menge
