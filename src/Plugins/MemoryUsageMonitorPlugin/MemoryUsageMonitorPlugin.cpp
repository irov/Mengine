#include "MemoryUsageMonitorPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MemoryUsageMonitor, Mengine::MemoryUsageMonitorPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryUsageMonitorPlugin::MemoryUsageMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryUsageMonitorPlugin::~MemoryUsageMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryUsageMonitorPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "memoryusagemonitor" ) == false )
        {
            return false;
        }

        if( HAS_OPTION( "nomemoryusagemonitor" ) == true )
        {
            return false;
        }

        bool MemoryUsageMonitorPlugin_Available = CONFIG_VALUE_BOOLEAN( "MemoryUsageMonitorPlugin", "Available", true );

        if( MemoryUsageMonitorPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryUsageMonitorPlugin::_initializePlugin()
    {
        m_memoryUsageMonitor = Helper::makeFactorableUnique<MemoryUsageMonitor>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_memoryUsageMonitor->initialize() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryUsageMonitorPlugin::_finalizePlugin()
    {
        m_memoryUsageMonitor->finalize();
        m_memoryUsageMonitor = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
