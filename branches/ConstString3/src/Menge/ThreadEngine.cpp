/*
 *	ThreadEngine.cpp
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ThreadEngine.h"
#	include <cassert>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::ThreadEngine()
		: m_threadSystemInterface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadEngine::~ThreadEngine()
	{
		if( m_threadSystemInterface != 0 )
		{
			releaseInterfaceSystem( m_threadSystemInterface );
			m_threadSystemInterface = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadEngine::initialize()
	{	
		bool result = initInterfaceSystem( &m_threadSystemInterface );
        //bool result = false;
		if( ( result == false )|| ( m_threadSystemInterface == 0 ) )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentity * ThreadEngine::createThread( ThreadListener * _threadInterface )
	{
		ThreadIdentity * ident = m_threadSystemInterface->createThread( _threadInterface );
		return ident;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::joinThread( ThreadIdentity* _threadInterface )
	{
		m_threadSystemInterface->joinThread( _threadInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadEngine::sleep( unsigned int _ms )
	{
		m_threadSystemInterface->sleep( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadSystemInterface* ThreadEngine::getInterface() const
	{
		return m_threadSystemInterface;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
