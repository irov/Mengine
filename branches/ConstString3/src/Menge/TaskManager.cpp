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
		for( TTaskVector::iterator it = m_tasksToAdd.begin(),
			it_end = m_tasksToAdd.end();
			it != it_end;
		++it )
		{
			(*it)->destroy();
		}
		m_tasksToAdd.clear();

		for( TTaskVector::iterator it = m_tasksInProgress.begin(),
			it_end = m_tasksInProgress.end();
			it != it_end;
		++it )
		{
			Task*& task = (*it);
			task->cancel();
			ThreadManager::get()
				->joinThread( task );
			task->cleanup();
			task->destroy();
			//waitUntilDone( (*it) );
		}
		m_tasksInProgress.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::addTask( Task* _task )
	{
		m_tasksToAdd.push_back( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::update()
	{
		ThreadManager* threadMgr = ThreadManager::get();
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

		for( TTaskVector::iterator 
			it = m_tasksInProgress.begin();
		it != m_tasksInProgress.end(); )
		{
			if( (*it)->isComplete() == true )
			{
				Task*& task = (*it);
				task->postMain();
				task->destroy();

				it = m_tasksInProgress.erase( it );
			}
			else
			{
				++it;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::waitUntilDone( Task* _task )
	{
		TTaskVector::iterator it_add_find = std::find( m_tasksToAdd.begin(), m_tasksToAdd.end(), _task );
		if( it_add_find != m_tasksToAdd.end() )
		{
 			_task->preMain();
 			
			ThreadManager::get()
 				->createThread( _task );
 			
			ThreadManager::get()
 				->joinThread( _task );
			
			_task->destroy();

			m_tasksToAdd.erase( it_add_find );
			return;
		}

		TTaskVector::iterator it_progress_find = std::find( m_tasksInProgress.begin(), m_tasksInProgress.end(), _task );
		if( it_progress_find != m_tasksInProgress.end() )
		{
			ThreadManager::get()
				->joinThread( _task );

			_task->destroy();

			m_tasksInProgress.erase( it_progress_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
