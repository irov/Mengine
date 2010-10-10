/*
 *	Affector.cpp
 *
 *	Created by _Berserk_ on 2.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Affector.h"

#	include "Player.h"
#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Affector::Affector( PyObject* _cb, EAffectorType _type )
		: m_cb(_cb)
		, m_type(_type)
		, m_endFlag(true)		
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
	void Affector::setEndFlag( bool _endFlag )
	{
		m_endFlag = _endFlag;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::call( Scriptable * _scriptable )
	{
		if( m_cb != Py_None )
		{
			Player::get()
				->addCallback( m_cb, _scriptable->getEmbed(), m_endFlag );
		}
	}
}	// namespace Menge
