#	include "ThreadTaskDummy.h"

#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskDummy::ThreadTaskDummy()
		: m_type( 0 )
		, m_id( 0 )
		, m_receiver( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskDummy::initialize( uint32_t _type, HttpRequestID _id, HttpReceiver * _receiver )
	{
		m_type = _type;
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
		switch( m_type )
		{
		case 0:
			{
				m_receiver->onDownloadAssetComplete( m_id, nullptr, 0, false );
			}break;
		case 1:
			{
				m_receiver->onGetMessageComplete( m_id, "", 0, false );
			}break;
		case 2:
			{
				m_receiver->onPostMessageComplete( m_id, "", 0, false );
			}break;
		}
	}
}
