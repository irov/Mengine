#include "FactoryLeakDetectorPlugin.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "FactoryLeakDetectorServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( FactoryLeakDetectorService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( FactoryLeakDetector, Mengine::FactoryLeakDetectorPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryLeakDetectorPlugin::FactoryLeakDetectorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryLeakDetectorPlugin::~FactoryLeakDetectorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryLeakDetectorPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "memleak" ) == true )
        {
            return true;
        }

        if( CONFIG_VALUE( "FactoryLeakDetector", "Enable", false ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryLeakDetectorPlugin::_systemPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryLeakDetectorPlugin::_initializePlugin()
    {
        SERVICE_CREATE( FactoryLeakDetectorService );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryLeakDetectorPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( FactoryLeakDetectorService );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryLeakDetectorPlugin::_destroy()
    {
        SERVICE_DESTROY( FactoryLeakDetectorService );
    }
}