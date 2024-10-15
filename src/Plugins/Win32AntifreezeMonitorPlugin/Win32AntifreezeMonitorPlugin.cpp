#include "Win32AntifreezeMonitorPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32AntifreezeMonitorPlugin, Mengine::Win32AntifreezeMonitorPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitorPlugin::Win32AntifreezeMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32AntifreezeMonitorPlugin::~Win32AntifreezeMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AntifreezeMonitorPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "antifreezemonitor" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noantifreezemonitor" ) == true )
        {
            return false;
        }

        bool Win32AntifreezeMonitorPlugin_Available = CONFIG_VALUE( "Win32AntifreezeMonitorPlugin", "Available", true );

        if( Win32AntifreezeMonitorPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AntifreezeMonitorPlugin::_initializePlugin()
    {
        m_antifreezeMonitor = Helper::makeFactorableUnique<Win32AntifreezeMonitor>( MENGINE_DOCUMENT_FACTORABLE );

        m_antifreezeMonitor->initialize();

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_RUN, &Win32AntifreezeMonitorPlugin::notifyPlatformRun_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_UPDATE, &Win32AntifreezeMonitorPlugin::notifyPlatformUpdate_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_STOP, &Win32AntifreezeMonitorPlugin::notifyPlatformStop_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitorPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_RUN );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_UPDATE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_STOP );

        m_antifreezeMonitor->finalize();

        m_antifreezeMonitor = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitorPlugin::notifyPlatformRun_()
    {
        m_antifreezeMonitor->run();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitorPlugin::notifyPlatformUpdate_()
    {
        m_antifreezeMonitor->ping();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AntifreezeMonitorPlugin::notifyPlatformStop_()
    {
        m_antifreezeMonitor->stop();
    }
    //////////////////////////////////////////////////////////////////////////
}
