#include "AppleFirebasePlugin.h"

#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebase, Mengine::AppleFirebasePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebasePlugin::AppleFirebasePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebasePlugin::~AppleFirebasePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebasePlugin::_availablePlugin() const
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebasePlugin::_initializePlugin()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebasePlugin::_finalizePlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebasePlugin::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
