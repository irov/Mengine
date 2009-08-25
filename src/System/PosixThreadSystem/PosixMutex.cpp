/*
 *	PosixMutex.cpp
 *
 *	Created by _Berserk_ on 25.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "PosixMutex.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixMutex::PosixMutex()
		: m_pmid( 0 )
		, m_locked( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixMutex::~PosixMutex()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixMutex::initialize()
	{
		if( pthread_mutex_init( &m_pmid, NULL ) != 0 )
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixMutex::lock()
	{
		if( m_locked == false && pthread_mutex_lock( &m_pmid ) == 0 )
		{
			m_locked = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixMutex::unlock()
	{
		if( m_locked == true && pthread_mutex_unlock( &m_pmid ) == 0 )
		{
			m_locked = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixMutex::isLocked() const
	{
		return m_locked;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge