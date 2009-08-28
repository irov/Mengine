/*
 *	ThreadManager.h
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/ThreadSystemInterface.h"
#	include "Holder.h"

namespace Menge
{
	class ThreadManager
	{
	public:
		ThreadManager( ThreadSystemInterface* _threadSystemInterface );
		~ThreadManager();

	public:
		void createThread( ThreadInterface* _threadInterface );
		void joinThread( ThreadInterface* _threadInterface );
		void sleep( unsigned int _ms );
		MutexInterface* createMutex();
		void releaseMutex( MutexInterface* _mutex );
		
	protected:
		ThreadSystemInterface* m_threadSystemInterface;
		int m_mutexCount;
	};
}	// namespace Menge
