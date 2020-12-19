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
    void ThreadTaskPrefetch::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const PrefetcherObserverInterfacePtr & _observer )
    {
        m_fileGroup = _fileGroup;
        m_filePath = _filePath;
        m_observer = _observer;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::finalize()
    {
        this->_finalize();

        m_fileGroup = nullptr;
        m_stream = nullptr;
        m_observer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_onThreadTaskPreparation()
    {
        m_observer->onPrefetchPreparation();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetch::_onThreadTaskRun()
    {
        bool successful = m_observer->onPrefetchRun();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_onThreadTaskCancel()
    {
        if( m_observer != nullptr )
        {
            m_observer->onPrefetchCancel();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetch::_onThreadTaskComplete( bool _successful )
    {
        m_fileGroup = nullptr;
        m_stream = nullptr;

        m_observer->onPrefetchComplete( _successful );
        m_observer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
