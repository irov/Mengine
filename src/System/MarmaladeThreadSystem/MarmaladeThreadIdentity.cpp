#	include "MarmaladeThreadIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadIdentity::MarmaladeThreadIdentity()
		: m_thread(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadIdentity::initialize( s3eThread * _thread, const ThreadTaskInterfacePtr & _task )
    {
        m_thread = _thread;
		m_task = _task;
    }
	//////////////////////////////////////////////////////////////////////////
	s3eThread * MarmaladeThreadIdentity::getThread() const
	{
		return m_thread;
	}
}