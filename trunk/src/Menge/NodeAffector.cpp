/*
 *	NodeAffector.cpp
 *
 *	Created by _Berserk_ on 2.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "NodeAffector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeAffector::NodeAffector( PyObject* _endCallback, int _type )
		: m_endCallback(_endCallback)
		, m_endFlag(true)
		, m_type(_type)
	{
		pybind::incref( m_endCallback );		
	}
	//////////////////////////////////////////////////////////////////////////
	NodeAffector::~NodeAffector()
	{
		pybind::decref( m_endCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	int NodeAffector::getType() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeAffector::setEndFlag( bool _endFlag )
	{
		m_endFlag = _endFlag;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
