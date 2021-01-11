#include "Win32AntifreezeMonitor.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Interface/Win32PlatformExtensionInterface.h"
#endif

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionObservable.h"

#include "Kernel/Stringstream.h"

#include <iomanip>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitor::Win32AntifreezeMonitor()
        : m_seconds( 0 )
        , m_workerId( 0 )
        , m_refalive( 0 )
        , m_reflogger( 0 )
        , m_oldrefalive( 0 )
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
        bool enable = CONFIG_VALUE( "Engine", "AntifreezeMonitorEnable", true );

        if( enable == false )
        {
            return true;
        }

        uint32_t seconds = CONFIG_VALUE( "Engine", "AntifreezeMonitorSeconds", 10U );

        if( seconds == 0U )
        {
            return true;
        }

        m_seconds = seconds;

        ThreadJobPtr threadJob = THREAD_SERVICE()
            ->createJob( m_seconds * 1000, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadJob, "invalid create thread job" );

        m_threadJob = threadJob;

        if( THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ), MENGINE_THREAD_PRIORITY_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ), m_threadJob ) == false )
        {
            return false;
        }

        uint32_t workerId = m_threadJob->addWorker( ThreadWorkerInterfacePtr( this ), MENGINE_DOCUMENT_FACTORABLE );

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
        bool enable = CONFIG_VALUE( "Engine", "AntifreezeMonitorEnable", true );

        if( enable == false )
        {
            return;
        }

        uint32_t seconds = CONFIG_VALUE( "Engine", "AntifreezeMonitorSeconds", 10U );

        if( seconds == 0U )
        {
            return;
        }

        if( m_workerId != 0 )
        {
            m_threadJob->removeWorker( m_workerId );
            m_workerId = 0;
        }

        m_threadJob = nullptr;

        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ) );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_POST );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_LOGGER_BEGIN );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_LOGGER_END );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ABORT );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::ping()
    {
        m_refalive++;
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

        uint32_t refalive = m_refalive;
        uint32_t oldrefalive = m_oldrefalive;

        m_oldrefalive = refalive;

        if( oldrefalive == 0 )
        {
            return true;
        }

        if( oldrefalive != m_refalive )
        {
            return true;
        }

        if( m_reflogger != 0 )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true && HAS_OPTION( "antifreezemonitordebug" ) == false )
        {
            return true;
        }

#ifdef MENGINE_PLATFORM_WINDOWS
        Win32PlatformExtensionInterface * extension = PLATFORM_SERVICE()
            ->getPlatformExtention();

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

        PlatformDateTime dateTime;
        m_dateTimeProvider->getLocalDateTime( &dateTime );

        Stringstream ss_date;
        ss_date << dateTime.year
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.month
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.day
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

        String str_date = ss_date.str();

        processDumpPath += str_date;
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

        MENGINE_ERROR_FATAL( "Antifreeze monitor detect freeze process for [%u] seconds, and create dump '%s' [scene process: %s]"
            , m_seconds
            , processDumpPath.c_str()
            , sceneProcess == true ? "yes" : "no"
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

        ++m_reflogger;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyChangeLocalePost( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        MENGINE_ASSERTION_FATAL( m_reflogger != 0 );

        --m_reflogger;
        
        m_refalive++;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyLoggerBegin( ELoggerLevel _level )
    {
        MENGINE_UNUSED( _level );

        ++m_reflogger;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyLoggerEnd( ELoggerLevel _level )
    {
        MENGINE_UNUSED( _level );

        MENGINE_ASSERTION_FATAL( m_reflogger != 0 );

        --m_reflogger;

        m_refalive++;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::notifyAbort( const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        m_reflogger += 1024;
    }
    //////////////////////////////////////////////////////////////////////////
}
