#	include "ThreadEngine.h"
#	include "LogEngine.h"

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
			m_threadSystemInterface->finalize();

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

		LogSystemInterface * logSystem = LogEngine::get()
			->getInterface();

		m_threadSystemInterface->initialize( logSystem );
		
		return true;
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
