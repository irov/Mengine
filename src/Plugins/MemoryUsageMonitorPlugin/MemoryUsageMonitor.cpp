#include "MemoryUsageMonitor.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/AllocatorSystemInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/UnknownAllocatorDebugReportInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/StatisticHelper.h"

#include "Config/Algorithm.h"

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
        uint32_t MemoryUsageMonitor_Seconds = CONFIG_VALUE( "MemoryUsageMonitorPlugin", "Seconds", 10U );

        MENGINE_ASSERTION_FATAL( MemoryUsageMonitor_Seconds != 0 );

        m_seconds = MemoryUsageMonitor_Seconds;

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

        uint32_t totalMemoryUsage = ALLOCATOR_SYSTEM()
            ->getMemoryUsage();

        const ConstString & currentSceneName = STATISTIC_GET_CONSTSTRING( "CURRENT_SCENE_NAME" );

        LOGGER_MESSAGE_RELEASE( "Memory usage monitor [%ugb %umb %ukb] [scene %s]"
            , (totalMemoryUsage / (1000 * 1000 * 1000))
            , ((totalMemoryUsage / (1000 * 1000)) % 1000)
            , ((totalMemoryUsage / 1000) % 1000)
            , currentSceneName.c_str()
        );

        UnknownAllocatorDebugReportInterface * debugReport = ALLOCATOR_SYSTEM()
            ->getUnknown();

        if( debugReport != nullptr )
        {
            uint32_t allocatorReportCount = debugReport->getAllocatorReportCount();

            struct MemoryUsageDesc
            {
                String name;
                size_t count;
            };

            typedef Vector<MemoryUsageDesc> VectorMemoryUsages;
            VectorMemoryUsages usages;

            for( uint32_t index = 0; index != allocatorReportCount; ++index )
            {
                const Char * reportName;
                size_t reportCount = debugReport->getAllocatorReportInfo( index, &reportName );

                if( reportName[0] == '\0' )
                {
                    continue;
                }

                if( reportCount < 1024 * 1024 )
                {
                    continue;
                }

                usages.emplace_back( MemoryUsageDesc{reportName, reportCount} );
            }

            Algorithm::sort( usages.begin(), usages.end(), []( const MemoryUsageDesc & _l, const MemoryUsageDesc & _r )
            {
                return _l.count > _r.count;
            } );

            for( const MemoryUsageDesc & desc : usages )
            {
                LOGGER_MESSAGE_RELEASE( "  - %s [%zumb %zukb]"
                    , desc.name.c_str()
                    , (desc.count / (1000 * 1000))
                    , ((desc.count / 1000) % 1000)
                );
            }
        }

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
