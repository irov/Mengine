#include "ThreadTaskDummy.h"

#include "Interface/FileSystemInterface.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskDummy::ThreadTaskDummy()
		: m_id( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskDummy::initialize( HttpRequestID _id, const HttpReceiverInterfacePtr & _receiver )
	{
		m_id = _id;
		m_receiver = _receiver;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskDummy::_onRun()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskDummy::_onMain()
	{		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskDummy::_onComplete( bool _successful )
	{
		(void)_successful;

        m_receiver->onHttpRequestComplete( m_id, 0, "", 0, false );
	}
}
