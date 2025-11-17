#include "LayoutEditorPlugin.h"

#include "Interface/ScriptServiceInterface.h"

#include "LayoutEditorModule.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#include "LayoutEditorScriptEmbedding.h"
#endif

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/NotificationHelper.h"

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

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [MENGINE_DOCUMENT_ARGUMENTS( this )]()
            {
                SCRIPT_SERVICE()
                    ->addScriptEmbedding( LayoutEditorScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<LayoutEditorScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
            }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
            {
                SCRIPT_SERVICE()
                    ->removeScriptEmbedding( LayoutEditorScriptEmbedding::getFactorableType() );
            }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorPlugin::_finalizePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        this->removeModuleFactory( LayoutEditorModule::getFactorableType() );
    }
    //////////////////////////////////////////////////////////////////////////
}