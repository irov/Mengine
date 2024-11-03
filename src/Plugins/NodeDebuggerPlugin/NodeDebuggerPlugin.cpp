#include "NodeDebuggerPlugin.h"

#include "NodeDebuggerModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( NodeDebugger, Mengine::NodeDebuggerPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerPlugin::NodeDebuggerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerPlugin::~NodeDebuggerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleNodeDebugger" ), Helper::makeModuleFactory<NodeDebuggerModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerPlugin::_finalizePlugin()
    {
        this->removeModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleNodeDebugger" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
