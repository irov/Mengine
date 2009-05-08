/*
 *	TaskManager.cpp
 *
 *	Created by _Berserk_ on 24.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "TaskManager.h"
#	include "Task.h"
#	include "ThreadManager.h"

#	include "Functor.h"

#	include <algorithm>
#	include <functional>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TaskManager::TaskManager( )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TaskManager::~TaskManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::addTask( Task* _task )
	{
		_task->preMain();

		Holder<ThreadManager>::hostage()
			->createThread( _task );

		m_tasksToAdd.push_back( _task );
		//m_tasksInProgress.push_back( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::update()
	{
		if( m_tasksToAdd.empty() == false )
		{
			m_tasksInProgress.insert( m_tasksInProgress.end(), m_tasksToAdd.begin(), m_tasksToAdd.end() );
			m_tasksToAdd.clear();
		}

		if( m_tasksInProgress.empty() == true )
		{
			return;
		}

		std::for_each( m_tasksInProgress.begin()
						, m_tasksInProgress.end()
						, std::mem_fun( &Task::update ) );

		TTaskVector::iterator it_remove = std::stable_partition( 
			m_tasksInProgress.begin(), 
			m_tasksInProgress.end(), 
			std::mem_fun( &Task::isComplete ) 
			);

		std::for_each( m_tasksInProgress.begin()
						, it_remove
						, std::mem_fun( &Task::destroy ) 
						);

		m_tasksInProgress.erase( m_tasksInProgress.begin(), it_remove );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::waitUntilDone( Task* _task )
	{
		TTaskVector::iterator it_find = std::find( m_tasksInProgress.begin(), m_tasksInProgress.end(), _task );
		if( it_find != m_tasksInProgress.end() )
		{
			Holder<ThreadManager>::hostage()
				->joinThread( _task );
			_task->destroy();
			m_tasksInProgress.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
