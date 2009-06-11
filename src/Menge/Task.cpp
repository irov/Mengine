///*
// *	Task.cpp
// *
// *	Created by _Berserk_ on 24.4.2009
// *	Copyright 2009 Menge. All rights reserved.
// *
// */
//
#	include "Task.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Task::Task()
		: m_complete( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Task::~Task()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Task::destroy()
	{
		//postMain();
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Task::isComplete() const 
	{
		return m_complete;
	}
	//////////////////////////////////////////////////////////////////////////
	void Task::setName( const String & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Task::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Task::preMain()
	{
		//Empty
	};
	//////////////////////////////////////////////////////////////////////////
	void Task::postMain()
	{
		//Empty
	};
	//////////////////////////////////////////////////////////////////////////
	void Task::update()
	{
		//Empty
	};
	//////////////////////////////////////////////////////////////////////////
	void Task::cancel()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Task::cleanup()
	{

	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
