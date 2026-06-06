#include "GLTFImporterPlugin.h"
#include "GLTFImporterService.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/PluginHelper.h"

SERVICE_EXTERN( GLTFImporterService );

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( GLTFImporter, Mengine::GLTFImporterPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GLTFImporterPlugin::GLTFImporterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GLTFImporterPlugin::~GLTFImporterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GLTFImporterPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nogltf" ) == true )
        {
            return false;
        }

        bool GLTFImporterPlugin_Available = CONFIG_VALUE_BOOLEAN( "GLTFImporterPlugin", "Available", true );

        if( GLTFImporterPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GLTFImporterPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( GLTFImporterService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GLTFImporterPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( GLTFImporterService );
    }
    //////////////////////////////////////////////////////////////////////////
    void GLTFImporterPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( GLTFImporterService );
    }
    //////////////////////////////////////////////////////////////////////////
}
