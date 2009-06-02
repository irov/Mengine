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

#	include <cassert>
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
		const String & newtaskName = _task->getName();
		for( TTaskVector::iterator it = m_tasksToAdd.begin(),
			it_end = m_tasksToAdd.end();
			it != it_end;
		++it )
		{
			const String & curtaskName = (*it)->getName();
			if( newtaskName == curtaskName )
			{
				assert(!"duplicate thread task");
				return;
			}
		}

		for( TTaskVector::iterator it = m_tasksInProgress.begin(),
			it_end = m_tasksInProgress.end();
			it != it_end;
		++it )
		{
			const String & curtaskName = (*it)->getName();
			if( newtaskName == curtaskName )
			{
				assert(!"duplicate thread task");
				return;
			}
		}

		m_tasksToAdd.push_back( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::update()
	{
		ThreadManager* threadMgr = Holder<ThreadManager>::hostage();
		for( TTaskVector::iterator it = m_tasksToAdd.begin(), it_end = m_tasksToAdd.end();
			it != it_end;
			++it )
		{
			Task*& task = (*it);
			task->preMain();
			threadMgr->createThread( task );
			m_tasksInProgress.push_back( task );
		}
		m_tasksToAdd.clear();

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
		TTaskVector::iterator it_find = std::find( m_tasksToAdd.begin(), m_tasksToAdd.end(), _task );
		if( it_find != m_tasksToAdd.end() )
		{
			_task->preMain();
			Holder<ThreadManager>::hostage()
				->createThread( _task );
			Holder<ThreadManager>::hostage()
				->joinThread( _task );
			_task->destroy();
			m_tasksToAdd.erase( it_find );
			return;
		}
		// else
		it_find = std::find( m_tasksInProgress.begin(), m_tasksInProgress.end(), _task );
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
