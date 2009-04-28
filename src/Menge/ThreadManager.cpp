/*
 *	ThreadManager.cpp
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ThreadManager.h"

namespace Menge
{

	//////////////////////////////////////////////////////////////////////////
	ThreadManager::ThreadManager( ThreadSystemInterface* _threadSystemInterface )
		: m_threadSystemInterface( _threadSystemInterface )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadManager::~ThreadManager()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadManager::createThread( ThreadInterface* _threadInterface )
	{
		m_threadSystemInterface->createThread( _threadInterface );
	}
	//////////////////////////////////////////////////////////////////////////

}	// namespace Menge
