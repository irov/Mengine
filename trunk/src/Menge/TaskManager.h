/*
 *	TaskManager.h
 *
 *	Created by _Berserk_ on 24.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Holder.h"
#	include <vector>

namespace Menge
{
	class Task;

	class TaskManager
	{
	public:
		TaskManager();
		~TaskManager();

	public:
		void addTask( Task* _task );
		void update();
		void waitUntilDone( Task* _task );

	protected:

		typedef std::vector<Task*> TTaskVector;
		TTaskVector m_tasksInProgress;
		TTaskVector m_tasksToAdd;
	};
}	// namespace Menge
