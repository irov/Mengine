#include "Win32AntifreezeMonitor.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "Kernel/Stringstream.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitor::Win32AntifreezeMonitor()
        : m_seconds( 0 )
        , m_workerId( 0 )
        , m_refalive( 0 )
        , m_oldrefalive( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitor::~Win32AntifreezeMonitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AntifreezeMonitor::initialize()
    {
        uint32_t seconds = CONFIG_VALUE( "Engine", "AntifreezeMonitorSeconds", 5U );

        if( seconds == 0U )
        {
            return true;
        }

        m_seconds = seconds;

        ThreadJobPtr threadJob = THREAD_SERVICE()
            ->createJob( seconds * 1000, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadJob, false, "invalid create thread job" );

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

        uint32_t workerId = m_threadJob->addWorker( ThreadWorkerInterfacePtr( this ) );

        m_workerId = workerId;

        DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
            ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( dateTimeProvider, false );

        m_dateTimeProvider = dateTimeProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::finalize()
    {
        if( m_workerId != 0 )
        {
            m_threadJob->removeWorker( m_workerId );
            m_threadJob = nullptr;
        }

        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::ping()
    {
        m_refalive++;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::onUpdate( uint32_t _id )
    {
        MENGINE_UNUSED( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AntifreezeMonitor::onWork( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        uint32_t oldrefalive = m_oldrefalive;
        m_oldrefalive = m_refalive;

        if( oldrefalive == 0 )
        {
            return true;
        }

        if( oldrefalive != m_refalive )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true )
        {
            return true;
        }

        Char userPath[MENGINE_MAX_PATH] = { '\0' };
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

        PLATFORM_SERVICE()
            ->createProcessDump( processDumpPath.c_str(), nullptr, true );

        MENGINE_ERROR_MESSAGE( "Antifreeze monitor detect freeze process for %u seconds, and create dump '%s'"
            , m_seconds
            , processDumpPath.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::onDone( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        //Empty
    }
}
