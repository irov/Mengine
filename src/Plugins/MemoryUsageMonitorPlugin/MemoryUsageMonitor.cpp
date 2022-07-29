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
#include "Kernel/NotificationHelper.h"
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

        m_mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, &MemoryUsageMonitor::notifyChangeSceneInitialize, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY, &MemoryUsageMonitor::notifyChangeSceneDestroy, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryUsageMonitor::finalize()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_INITIALIZE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY );

        if( m_workerId != 0 )
        {
            m_threadJob->removeWorker( m_workerId );
            m_workerId = 0;
        }

        m_threadJob = nullptr;

        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "MemoryUsageMonitor" ) );

        m_mutex = nullptr;
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

        size_t totalMemoryUsage = ALLOCATOR_SERVICE()
            ->get_report_total();

        m_mutex->lock();
        ConstString currentSceneName = m_currentSceneName;
        m_mutex->unlock();

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
    void MemoryUsageMonitor::notifyChangeSceneInitialize( const ScenePtr & _newScene )
    {
        m_mutex->lock();
        m_currentSceneName = _newScene->getName();
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryUsageMonitor::notifyChangeSceneDestroy( const ScenePtr & _oldCcene )
    {
        MENGINE_UNUSED( _oldCcene );

        m_mutex->lock();
        m_currentSceneName = ConstString::none();
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}
