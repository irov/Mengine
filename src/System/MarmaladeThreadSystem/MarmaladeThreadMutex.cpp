#   include "MarmaladeThreadMutex.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeThreadMutex::MarmaladeThreadMutex()
        : m_serviceProvider(nullptr)
        , m_cs(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        m_cs = s3eThreadLockCreate();
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::lock()
    {
        s3eThreadLockAcquire( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::unlock()
    {
        s3eThreadLockRelease( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::_destroy()
    {
        s3eThreadLockDestroy( m_cs );
    }
}