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

		m_tasksInProgress.push_back( _task );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::update()
	{
		if( m_tasksInProgress.empty() == true )
		{
			return;
		}

		std::for_each( m_tasksInProgress.begin(), m_tasksInProgress.end(), std::mem_fun( &Task::update ) );

		TTaskVector::iterator it_remove = std::remove_if( 
			m_tasksInProgress.begin(), 
			m_tasksInProgress.end(), 
			std::mem_fun( &Task::isComplete ) 
			);

		std::for_each( it_remove, m_tasksInProgress.end(), std::mem_fun( &Task::destroy ) );

		//TTaskVector::iterator it_real_end = m_tasksInProgress.end();
		//TTaskVector::iterator it_end = it_real_end;
		//Task*& tail = (*(it_end-1));
		//for( TTaskVector::iterator it = m_tasksInProgress.begin();
		//	it != it_end;
		//	/**/ )
		//{
		//	Task*& t = (*it);
		//	t->update();
		//	if( t->isComplete() == true )
		//	{
		//		t->postMain();
		//		delete t;
		//		std::swap( t, tail );
		//		tail = (*((--it_end)-1));
		//	}
		//	else
		//	{
		//		++it;
		//	}
		//}

		// erase done tasks
		//m_tasksInProgress.erase( it_end, it_real_end );

	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
