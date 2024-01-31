#include "MemoryUsageMonitor.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/AllocatorSystemInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
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
            ->createThreadProcessor( STRINGIZE_STRING_LOCAL( "MemoryUsageMonitor" ), ETP_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
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
            ->destroyThreadProcessor( STRINGIZE_STRING_LOCAL( "MemoryUsageMonitor" ) );

        m_prevUsages.clear();
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

        LOGGER_SERVICE()
            ->lockMessage();

        uint32_t totalMemoryUsage = ALLOCATOR_SYSTEM()
            ->getMemoryUsage();

        const ConstString & currentSceneName = STATISTIC_GET_CONSTSTRING( STATISTIC_CURRENT_SCENE_NAME );

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
                VectorMemoryUsages::const_iterator it_found = Algorithm::find_if( m_prevUsages.begin(), m_prevUsages.end(), [desc]( const MemoryUsageDesc & _desc )
                {
                    return _desc.name == desc.name;
                } );

                if( it_found == m_prevUsages.end() )
                {
                    LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, nullptr, 0, Mengine::LFLAG_NONE )("  - %s [%zumb %zukb]"
                        , desc.name.c_str()
                        , (desc.count / (1000 * 1000))
                        , ((desc.count / 1000) % 1000)
                    );
                }
                else
                {
                    const MemoryUsageDesc & prevDesc = *it_found;

                    if( desc.count > prevDesc.count )
                    {
                        LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0, Mengine::LFLAG_NONE )("  - %s [%zumb %zukb] +%zukb"
                            , desc.name.c_str()
                            , (desc.count / (1000 * 1000))
                            , ((desc.count / 1000) % 1000)
                            , ((desc.count - prevDesc.count) / 1000)
                        );
                    }
                    else if( desc.count < prevDesc.count )
                    {
                        LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN, nullptr, 0, Mengine::LFLAG_NONE )("  - %s [%zumb %zukb] -%zukb"
                            , desc.name.c_str()
                            , (desc.count / (1000 * 1000))
                            , ((desc.count / 1000) % 1000)
                            , ((prevDesc.count - desc.count) / 1000)
                        );
                    }
                    else
                    {
                        LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::LFLAG_NONE )("  - %s [%zumb %zukb]"
                            , desc.name.c_str()
                            , (desc.count / (1000 * 1000))
                            , ((desc.count / 1000) % 1000)
                            );
                    }
                }
            }

            m_prevUsages = usages;
        }

        LOGGER_SERVICE()
            ->unlockMessage();

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
