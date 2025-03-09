#include "AppleFirebasePerformanceMonitoringPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

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
    bool AppleFirebasePerformanceMonitoringPlugin::_initializePlugin()
    {
        //ToDo

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebasePerformanceMonitoringPlugin::_finalizePlugin()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
}
