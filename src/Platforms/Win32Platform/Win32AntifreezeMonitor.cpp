#include "Win32AntifreezeMonitor.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/Stringstream.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitor::Win32AntifreezeMonitor()
        : m_refalive( 0 )
        , m_oldrefalive( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitor::~Win32AntifreezeMonitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AntifreezeMonitor::run()
    {
        uint32_t seconds = CONFIG_VALUE( "Engine", "AntifreezeMonitorSeconds", 5U );

        if( seconds == 0U )
        {
            return true;
        }

        m_seconds = seconds;

        ThreadJobPtr threadJob = THREAD_SERVICE()
            ->createJob( seconds * 1000, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( threadJob, false, "invalid create thread job" );

        m_threadJob = threadJob;

        if( THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ), MENGINE_THREAD_PRIORITY_NORMAL, MENGINE_DOCUMENT_FUNCTION ) == false )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::stop()
    {
        m_threadJob->removeWorker( m_workerId );        
        m_threadJob = nullptr;

        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "Win32AntifreezeMonitor" ), false );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitor::ping()
    {
        m_refalive++;
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
        
        Char userPath[MENGINE_MAX_PATH] = { 0 };
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        String processDumpPath;
        processDumpPath += userPath;
        processDumpPath += "Antifreeze";
        processDumpPath += "_";

        PlatformDateTime dateTime;
        PLATFORM_SERVICE()
            ->getDateTime( &dateTime );

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

        LOGGER_ERROR( "Antifreeze monitor detect freeze process for %u seconds, and create dump '%s'!"
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
