#include "MemoryUsageMonitor.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryUsageMonitor::MemoryUsageMonitor()
        : m_seconds( 0 )
        , m_workerId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryUsageMonitor::~MemoryUsageMonitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryUsageMonitor::initialize()
    {
        uint32_t Engine_MemoryUsageMonitorSeconds = CONFIG_VALUE( "Engine", "MemoryUsageMonitorSeconds", 10U );

        MENGINE_ASSERTION_FATAL( Engine_MemoryUsageMonitorSeconds != 0 );

        m_seconds = Engine_MemoryUsageMonitorSeconds;

        ThreadJobPtr threadJob = THREAD_SERVICE()
            ->createJob( m_seconds * 1000, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadJob, "invalid create thread job" );

        m_threadJob = threadJob;

        if( THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "MemoryUsageMonitor" ), ETP_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "MemoryUsageMonitor" ), m_threadJob, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        UniqueId workerId = m_threadJob->addWorker( ThreadWorkerInterfacePtr( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_workerId = workerId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryUsageMonitor::finalize()
    {
        if( m_workerId != 0 )
        {
            m_threadJob->removeWorker( m_workerId );
            m_workerId = 0;
        }

        m_threadJob = nullptr;

        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "MemoryUsageMonitor" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryUsageMonitor::onThreadWorkerUpdate( uint32_t _id )
    {
        MENGINE_UNUSED( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryUsageMonitor::onThreadWorkerWork( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        const ConstString & currentSceneName = SCENE_SERVICE()
            ->getCurrentSceneNameThreadSafe();

        size_t totalMemoryUsage = ALLOCATOR_SERVICE()
            ->get_report_total();

        LOGGER_MESSAGE_RELEASE( "Memory usage monitor [%zugb %zumb %zukb] [scene %s]"
            , (totalMemoryUsage / (1024 * 1024 * 1024))
            , ((totalMemoryUsage / (1024 * 1024)) % 1024)
            , (totalMemoryUsage / (1024) % 1024)
            , currentSceneName.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryUsageMonitor::onThreadWorkerDone( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
