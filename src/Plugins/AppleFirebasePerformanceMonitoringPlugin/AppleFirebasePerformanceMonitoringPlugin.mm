#include "AppleFirebasePerformanceMonitoringPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleFirebasePerformanceMonitoringService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebasePerformanceMonitoring, Mengine::AppleFirebasePerformanceMonitoringPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebasePerformanceMonitoringPlugin::AppleFirebasePerformanceMonitoringPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebasePerformanceMonitoringPlugin::~AppleFirebasePerformanceMonitoringPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebasePerformanceMonitoringPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applefirebaseperformancemonitoring" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noapplefirebaseperformancemonitoring" ) == true )
        {
            return false;
        }
        
        bool AppleFirebasePerformanceMonitoringPlugin_Available = CONFIG_VALUE_BOOLEAN( "AppleFirebasePerformanceMonitoringPlugin", "Available", true );

        if( AppleFirebasePerformanceMonitoringPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebasePerformanceMonitoringPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleFirebasePerformanceMonitoringService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebasePerformanceMonitoringPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleFirebasePerformanceMonitoringService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebasePerformanceMonitoringPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleFirebasePerformanceMonitoringService );
    }
    //////////////////////////////////////////////////////////////////////////
}
