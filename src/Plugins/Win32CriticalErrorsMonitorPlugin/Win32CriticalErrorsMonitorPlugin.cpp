#include "Win32CriticalErrorsMonitorPlugin.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#include "Environment/Windows/Win32GetCallstack.h"
#include "Environment/Windows/Win32CreateProcessDump.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

#include "Config/StdIO.h"
#include "Config/Path.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32CriticalErrorsMonitorPlugin, Mengine::Win32CriticalErrorsMonitorPlugin );
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
        if( HAS_OPTION( "criticalerrorsmonitor" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nocriticalerrorsmonitor" ) == true )
        {
            return false;
        }

        bool Win32CriticalErrorsMonitorPlugin_Available = CONFIG_VALUE_BOOLEAN( "Win32CriticalErrorsMonitorPlugin", "Available", true );

        if( Win32CriticalErrorsMonitorPlugin_Available == false )
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

        const Char * dumpPath = g_monitor->m_dumpPath.c_str();

        if( Helper::Win32CreateProcessDump( dumpPath, pExceptionPointers, false ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG WINAPI Win32CriticalErrorsMonitorPlugin::s_exceptionHandler( EXCEPTION_POINTERS * pExceptionPointers )
    {
        MENGINE_UNUSED( pExceptionPointers );

        LOGGER_ERROR( "exception catch" );

#if defined(MENGINE_PLATFORM_WINDOWS)
        Char stack[8096 + 1] = {'\0'};
        if( Helper::Win32GetCallstack( ~0U, pExceptionPointers->ContextRecord, stack, 8096 ) == false )
        {
            LOGGER_FATAL( "catch exception and write dumb '%s'"
                , g_monitor->m_dumpPath.c_str()
            );
        }
        else
        {
            LOGGER_FATAL( "catch exception and write dumb '%s'\n\n\n%s\n\n\n"
                , g_monitor->m_dumpPath.c_str()
                , stack
            );
        }

        Win32CriticalErrorsMonitorPlugin::s_writeCrashDump( pExceptionPointers );
#endif

        return EXCEPTION_EXECUTE_HANDLER;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CriticalErrorsMonitorPlugin::_initializePlugin()
    {
        Path userPath = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        m_dumpPath.format( "%sDump_%u_%u_%u_%u_%u_%u.dmp"
            , userPath
            , dateTime.year
            , dateTime.month
            , dateTime.day
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        g_monitor = this;

        if( ::IsDebuggerPresent() == TRUE )
        {
            return true;
        }

        ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
        ::SetUnhandledExceptionFilter( &Win32CriticalErrorsMonitorPlugin::s_exceptionHandler );

        LOGGER_INFO( "system", "start critical errors monitor plugin" );
        LOGGER_INFO( "system", "dump path: '%s'"
            , m_dumpPath.c_str()
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
