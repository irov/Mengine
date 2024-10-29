#include "DebugPanelPlugin.h"

#include "ModuleDebugPanel.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DebugPanel, Mengine::DebugPanelPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DebugPanelPlugin::DebugPanelPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DebugPanelPlugin::~DebugPanelPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DebugPanelPlugin::_availablePlugin() const
    {
        bool DebugPanelPlugin_Available = CONFIG_VALUE( "DebugPanelPlugin", "Available", true );

        if( DebugPanelPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DebugPanelPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleDebugPanel" ), Helper::makeModuleFactory<ModuleDebugPanel>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelPlugin::_finalizePlugin()
    {
        this->removeModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleDebugPanel" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}