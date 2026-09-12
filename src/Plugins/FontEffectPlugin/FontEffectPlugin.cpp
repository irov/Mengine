#include "FontEffectPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/DataServiceInterface.h"

#include "FontEffectService.h"
#include "FontEffectBase.h"
#include "FontEffectFile.h"
#include "FontEffectDataflow.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( FontEffectService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( FontEffect, Mengine::FontEffectPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontEffectPlugin::FontEffectPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectPlugin::~FontEffectPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( FontEffectService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FontEffectBase" ), Helper::makeFactorableUnique<DefaultPrototypeGenerator<FontEffectBase, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FontEffectFile" ), Helper::makeFactorableUnique<DefaultPrototypeGenerator<FontEffectFile, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            FontEffectDataflowPtr dataflowFontEffect = Helper::makeFactorableUnique<FontEffectDataflow>( MENGINE_DOCUMENT_FACTORABLE );

            if( dataflowFontEffect->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "fontEffect" ), dataflowFontEffect, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflowFontEffect = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "fontEffect" ) );
            dataflowFontEffect->finalize();
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FontEffectBase" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FontEffectFile" ), nullptr );

        SERVICE_FINALIZE( FontEffectService );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( FontEffectService );
    }
    //////////////////////////////////////////////////////////////////////////
}
