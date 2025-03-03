#include "TexturepackerPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "ResourceTexturepacker.h"
#include "ResourceMultiTexturepacker.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "TexturepackerScriptEmbedding.h"
#endif

#include "LoaderResourceTexturepacker.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Texturepacker, Mengine::TexturepackerPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TexturepackerPlugin::TexturepackerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TexturepackerPlugin::~TexturepackerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TexturepackerPlugin::_initializePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( TexturepackerScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<TexturepackerScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( TexturepackerScriptEmbedding::getFactorableType() );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( Helper::addResourcePrototype<ResourceTexturepacker, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceMultiTexturepacker, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( LoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceTexturepacker::getFactorableType(), Helper::makeFactorableUnique<LoaderResourceTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( LoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceTexturepacker::getFactorableType() );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        Helper::removeResourcePrototype<ResourceTexturepacker>();
        Helper::removeResourcePrototype<ResourceMultiTexturepacker>();
    }
    //////////////////////////////////////////////////////////////////////////
}
