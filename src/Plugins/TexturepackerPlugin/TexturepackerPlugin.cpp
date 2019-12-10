#include "TexturepackerPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "ResourceTexturepacker.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "TexturepackerScriptEmbedding.h"
#endif

#include "LoaderResourceTexturepacker.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"

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
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [this]()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "TexturepackerScriptEmbedding" ), Helper::makeFactorableUnique<TexturepackerScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        } );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "TexturepackerScriptEmbedding" ) );
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceTexturepacker, 64>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        SERVICE_WAIT( LoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ), Helper::makeFactorableUnique<LoaderResourceTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ) );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ) );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ) );
    }
}