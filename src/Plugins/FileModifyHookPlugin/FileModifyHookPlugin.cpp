#include "FileModifyHookPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( FileModifyHookService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( FileModifyHook, Mengine::FileModifyHookPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FileModifyHookPlugin::FileModifyHookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FileModifyHookPlugin::~FileModifyHookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileModifyHookPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileModifyHookPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "filemodifyhook" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nofilemodifyhook" ) == true )
        {
            return false;
        }

        bool FileModifyHookPlugin_Available = CONFIG_VALUE( "FileModifyHookPlugin", "Available", true );

        if( FileModifyHookPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileModifyHookPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( FileModifyHookService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileModifyHookPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( FileModifyHookService );
    }
    //////////////////////////////////////////////////////////////////////////
    void FileModifyHookPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( FileModifyHookService );
    }
    //////////////////////////////////////////////////////////////////////////
}
