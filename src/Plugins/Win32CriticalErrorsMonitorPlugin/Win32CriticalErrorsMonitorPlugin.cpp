#include "Win32CriticalErrorsMonitorPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Interface/Win32PlatformExtensionInterface.h"

#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32CriticalErrorsMonitorPlugin, Mengine::Win32CriticalErrorsMonitorPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32CriticalErrorsMonitorPlugin::Win32CriticalErrorsMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32CriticalErrorsMonitorPlugin::~Win32CriticalErrorsMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CriticalErrorsMonitorPlugin::_availablePlugin() const
    {
        if( CONFIG_VALUE( "Platform", "CriticalErrorsMonitor", true ) == false )
        {
            return false;
        }

        if( HAS_OPTION( "nocriticalerrorsmonitor" ) == true )
        {
            return false;
        }

        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static Win32CriticalErrorsMonitorPlugin * g_monitor = nullptr;
    //////////////////////////////////////////////////////////////////////////
    bool Win32CriticalErrorsMonitorPlugin::s_writeCrashDump( EXCEPTION_POINTERS * pExceptionPointers )
    {
        if( g_monitor == nullptr )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->createProcessDump( g_monitor->m_dumpPath, pExceptionPointers, false ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void s_log_callstack( void * _ud, const char * _format, ... )
    {
        MENGINE_UNUSED( _ud );

        va_list argList;
        va_start( argList, _format );

        Char format_msg[4096] = {'\0'};
        MENGINE_VSNPRINTF( format_msg, 4096, _format, argList );

        va_end( argList );

        LOGGER_ERROR( "%s", format_msg );
    }
    //////////////////////////////////////////////////////////////////////////
    LONG WINAPI Win32CriticalErrorsMonitorPlugin::s_exceptionHandler( EXCEPTION_POINTERS * pExceptionPointers )
    {
        LOGGER_ERROR( "Exception catch" );

        Win32PlatformExtensionInterface * extension = PLATFORM_SERVICE()
            ->getPlatformExtention();

        Char stack[8096] = {'\0'};
        if( extension->getCallstack( ~0U, stack, 8095, pExceptionPointers->ContextRecord ) == false )
        {
            LOGGER_CRITICAL( "Catch exception and write dumb '%s'"
                , g_monitor->m_dumpPath
            );
        }
        else
        {
            LOGGER_CRITICAL( "Catch exception and write dumb '%s'\n\n\n %s\n\n\n"
                , g_monitor->m_dumpPath
                , stack
            );
        }

        Win32CriticalErrorsMonitorPlugin::s_writeCrashDump( pExceptionPointers );

        return EXCEPTION_EXECUTE_HANDLER;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CriticalErrorsMonitorPlugin::_initializePlugin()
    {
        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
            ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        PlatformDateTime dateTime;
        dateTimeProvider->getLocalDateTime( &dateTime );

        MENGINE_SPRINTF( m_dumpPath, "%sDump_%u_%u_%u_%u_%u_%u.dmp"
            , userPath
            , dateTime.year
            , dateTime.month
            , dateTime.day
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        g_monitor = this;

        ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
        ::SetUnhandledExceptionFilter( &Win32CriticalErrorsMonitorPlugin::s_exceptionHandler );

        LOGGER_INFO( "system", "start critical errors monitor plugin" );
        LOGGER_INFO( "system", "dump path: '%s'"
            , m_dumpPath 
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32CriticalErrorsMonitorPlugin::_finalizePlugin()
    {
        g_monitor = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
