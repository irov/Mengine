#include "ThreadTaskPrefetch.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetch::ThreadTaskPrefetch()
        : m_realFileGroup( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetch::~ThreadTaskPrefetch()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::initialize( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer )
    {
        m_content = _content;
        m_observer = _observer;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::finalize()
    {
        this->_finalize();

        m_content = nullptr;
        m_observer = nullptr;

        m_stream = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_onThreadTaskPreparation()
    {
        PrefetcherObserverInterfacePtr observer = m_observer;
        observer->onPrefetchPreparation();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetch::_onThreadTaskRun()
    {
        PrefetcherObserverInterfacePtr observer = m_observer;
        bool successful = observer->onPrefetchRun();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_onThreadTaskCancel()
    {
        PrefetcherObserverInterfacePtr observer = m_observer;
        observer->onPrefetchCancel();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_onThreadTaskComplete( bool _successful )
    {
        m_content = nullptr;
        m_stream = nullptr;

        PrefetcherObserverInterfacePtr observer = m_observer;
        m_observer = nullptr;

        observer->onPrefetchComplete( _successful );
    }
    //////////////////////////////////////////////////////////////////////////
}
