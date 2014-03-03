#	include "MutexGuard.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MutexGuard::MutexGuard( const ThreadMutexInterfacePtr & _mutex )
		: m_mutex(_mutex)
	{
		m_mutex->lock();
	}
	//////////////////////////////////////////////////////////////////////////
	MutexGuard::~MutexGuard()
	{
		m_mutex->unlock();
	}
}