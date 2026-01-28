#include "LayoutEditorPlugin.h"

#include "LayoutEditorModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( LayoutEditor, Mengine::LayoutEditorPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LayoutEditorPlugin::LayoutEditorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutEditorPlugin::~LayoutEditorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorPlugin::_availablePlugin() const
    {
        bool LayoutEditorPlugin_Available = CONFIG_VALUE_BOOLEAN( "LayoutEditorPlugin", "Available", true );

        if( LayoutEditorPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorPlugin::_initializePlugin()
    {
        this->addModuleFactory( LayoutEditorModule::getFactorableType(), Helper::makeModuleFactory<LayoutEditorModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorPlugin::_finalizePlugin()
    {
        this->removeModuleFactory( LayoutEditorModule::getFactorableType() );
    }
    //////////////////////////////////////////////////////////////////////////
}