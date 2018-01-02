#	include "ThreadTaskPrefetch.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetch::ThreadTaskPrefetch()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetch::~ThreadTaskPrefetch()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetch::initialize( const ConstString& _pakName, const FilePath & _fileName, const PrefetcherObserverInterfacePtr & _observer )
	{
		m_pakName = _pakName;
		m_filePath = _fileName;
        m_observer = _observer;
	}
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_onPreparation()
    {
        m_observer->onPrefetchPreparation();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetch::_onRun()
    {
        bool successful = m_observer->onPrefetchRun();

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetch::_onComplete( bool _successful )
	{
		(void) _successful;

		m_group = nullptr;
		m_stream = nullptr;

        m_observer->onPrefetchComplete( _successful );
        m_observer = nullptr;
	}
}
