/*
 *	ThreadManager.cpp
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ThreadManager.h"
#	include <cassert>

namespace Menge
{

	//////////////////////////////////////////////////////////////////////////
	ThreadManager::ThreadManager()
		: m_threadSystemInterface( 0 )
		, m_mutexCount( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadManager::~ThreadManager()
	{
		assert( m_mutexCount == 0 );

		if( m_threadSystemInterface != 0 )
		{
			releaseInterfaceSystem( m_threadSystemInterface );
			m_threadSystemInterface = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadManager::initialize()
	{
		bool result = initInterfaceSystem( &m_threadSystemInterface );
		if( ( result == false )|| ( m_threadSystemInterface == 0 ) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadManager::createThread( ThreadInterface* _threadInterface )
	{
		m_threadSystemInterface->createThread( _threadInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadManager::joinThread( ThreadInterface* _threadInterface )
	{
		m_threadSystemInterface->joinThread( _threadInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadManager::sleep( unsigned int _ms )
	{
		m_threadSystemInterface->sleep( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
	MutexInterface* ThreadManager::createMutex()
	{
		MutexInterface* mutex = m_threadSystemInterface->createMutex();
		if( mutex != 0 )
		{
			++m_mutexCount;
		}
		return mutex;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadManager::releaseMutex( MutexInterface* _mutex )
	{
		if( _mutex != 0 )
		{
			assert( _mutex->isLocked() == false );
			--m_mutexCount;
			m_threadSystemInterface->releaseMutex( _mutex );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
