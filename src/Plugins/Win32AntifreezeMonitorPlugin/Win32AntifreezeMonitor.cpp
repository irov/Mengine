#include "Win32AntifreezeMonitor.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Interface/Win32PlatformExtensionInterface.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/StatisticHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitor::Win32AntifreezeMonitor()
        : m_seconds( 0 )
        , m_workerId( 0 )
        , m_refAlive( 0 )
        , m_refChangeLocale( 0 )
        , m_refLogger( 0 )
        , m_refAbort( 0 )
        , m_oldRefAlive( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitor::~Win32AntifreezeMonitor()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "Win32AntifreezeMonitor" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AntifreezeMonitor::initialize()
    {
        uint32_t Win32AntifreezeMonitorPlugin_Seconds = CONFIG_VALUE( "Win32AntifreezeMonitorPlugin", "Seconds", 10U );

        MENGINE_ASSERTION_FATAL( Win32AntifreezeMonitorPlugin_Seconds != 0 );

        m_seconds = Win32AntifreezeMonitorPlugin_Seconds;

        ThreadJobPtr threadJob = THREAD_SERVICE()
            ->createJob( m_seconds * 1000, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadJob, "invalid create thread job" );

        m_threadJob = threadJob;

        if( THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ), ETP_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ), m_threadJob, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        UniqueId workerId = m_threadJob->addWorker( ThreadWorkerInterfacePtr( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_workerId = workerId;

        DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
            ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( dateTimeProvider );

        m_dateTimeProvider = dateTimeProvider;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_LOCALE_PREPARE, &Win32AntifreezeMonitor::notifyChangeLocalePrepare, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_LOCALE_POST, &Win32AntifreezeMonitor::notifyChangeLocalePost, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_LOGGER_BEGIN, &Win32AntifreezeMonitor::notifyLoggerBegin, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_LOGGER_END, &Win32AntifreezeMonitor::notifyLoggerEnd, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ABORT, &Win32AntifreezeMonitor::notifyAbort, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::finalize()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_POST );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_LOGGER_BEGIN );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_LOGGER_END );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ABORT );

        if( m_workerId != 0 )
        {
            m_threadJob->removeWorker( m_workerId );
            m_workerId = 0;
        }

        m_threadJob = nullptr;

        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::ping()
    {
        ++m_refAlive;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::onThreadWorkerUpdate( uint32_t _id )
    {
        MENGINE_UNUSED( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AntifreezeMonitor::onThreadWorkerWork( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        uint32_t refalive = m_refAlive;
        uint32_t oldrefalive = m_oldRefAlive;

        m_oldRefAlive = refalive;

        if( oldrefalive == 0 )
        {
            return true;
        }

        if( oldrefalive != m_refAlive )
        {
            return true;
        }

        if( m_refChangeLocale != 0 )
        {
            return true;
        }

        if( m_refLogger != 0 )
        {
            return true;
        }

        if( m_refAbort != 0 )
        {
            return true;
        }

        if( HAS_OPTION( "antifreezemonitordebug" ) == false )
        {
            if( PLATFORM_SERVICE()
                ->isDebuggerPresent() == true )
            {
                return true;
            }
        }
        else
        {
            PLATFORM_SERVICE()
                ->debugBreak();
        }

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        Win32PlatformExtensionInterface * extension = PLATFORM_SERVICE()
            ->getUnknown();

        uint64_t mainThreadId = THREAD_SERVICE()
            ->getMainThreadId();

        Char stack_msg[8096] = {'\0'};
        if( extension->getCallstack( mainThreadId, stack_msg, 8095, nullptr ) == false )
        {
            LOGGER_ERROR( "Antifreeze monitor invalid callstack" );
        }
        else
        {
            LOGGER_ERROR( "Antifreeze monitor callstack:\n%s"
                , stack_msg
            );
        }
#endif

        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        String processDumpPath;
        processDumpPath += userPath;
        processDumpPath += "Antifreeze";
        processDumpPath += "_";

        Char filePathDate[1024] = {'\0'};
        Helper::makeFilePathDateTimestamp( m_dateTimeProvider, filePathDate, 1024 );

        processDumpPath += filePathDate;
        processDumpPath += ".dmp";

        if( PLATFORM_SERVICE()
            ->createProcessDump( processDumpPath.c_str(), nullptr, true ) == false )
        {
            LOGGER_ERROR( "Antifreeze monitor invalid create process dump '%s'"
                , processDumpPath.c_str()
            );
        }

        bool sceneProcess = SCENE_SERVICE()
            ->isProcess();

        const ConstString & currentSceneName = STATISTIC_GET_CONSTSTRING( "CURRENT_SCENE_NAME" );

        MENGINE_ERROR_FATAL( "Antifreeze monitor detect freeze process for [%u] seconds, and create dump '%s' [scene process: %s] [scene: %s]"
            , m_seconds
            , processDumpPath.c_str()
            , sceneProcess == true ? "yes" : "no"
            , currentSceneName.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::onThreadWorkerDone( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyChangeLocalePrepare( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        ++m_refChangeLocale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyChangeLocalePost( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        MENGINE_ASSERTION_FATAL( m_refChangeLocale != 0 );

        --m_refChangeLocale;
        
        ++m_refAlive;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyLoggerBegin( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _message, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _filter );
        MENGINE_UNUSED( _color );
        MENGINE_UNUSED( _message );
        MENGINE_UNUSED( _size );

        ++m_refLogger;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyLoggerEnd( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _message, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _filter );
        MENGINE_UNUSED( _color );
        MENGINE_UNUSED( _message );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_FATAL( m_refLogger != 0 );

        --m_refLogger;

        ++m_refAlive;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyAbort( const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        ++m_refAbort;
    }
    //////////////////////////////////////////////////////////////////////////
}
