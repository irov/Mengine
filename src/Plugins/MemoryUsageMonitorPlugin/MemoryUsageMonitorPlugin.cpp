#include "MemoryUsageMonitorPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MemoryUsageMonitorPlugin, Mengine::MemoryUsageMonitorPlugin )
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
        if( HAS_OPTION( "memoryusagemonitor" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nomemoryusagemonitor" ) == true )
        {
            return false;
        }

        if( CONFIG_VALUE( "Platform", "MemoryUsageMonitor", true ) == false )
        {
            return false;
        }

        bool Engine_MemoryUsageMonitor = CONFIG_VALUE( "Engine", "MemoryUsageMonitor", true );

        if( Engine_MemoryUsageMonitor == false )
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
