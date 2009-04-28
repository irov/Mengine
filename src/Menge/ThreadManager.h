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
		
	protected:
		ThreadSystemInterface* m_threadSystemInterface;
	};
}	// namespace Menge
