/*
 *	TaskFunctor.cpp
 *
 *	Created by _Berserk_ on 27.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "TaskFunctor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TaskFunctor::TaskFunctor( Functor& _functor )
		: Task()
		, m_functor( _functor )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void TaskFunctor::main()
	{
		m_functor();
		m_complete = true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
