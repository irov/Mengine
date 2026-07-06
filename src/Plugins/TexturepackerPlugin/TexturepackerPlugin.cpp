#include "TexturepackerPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "Interface/MetabufLoaderServiceInterface.h"
#endif

#include "ResourceTexturepacker.h"
#include "ResourceMultiTexturepacker.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "MetabufLoaderResourceTexturepacker.h"
#endif

#if defined(MENGINE_PLUGIN_JSON)
#include "JSONLoaderResourceTexturepacker.h"
#endif

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
        if( Helper::addResourcePrototype<ResourceTexturepacker, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceMultiTexturepacker, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#if defined(MENGINE_PLUGIN_METABUF)
        PLUGIN_SERVICE_WAIT( MetabufLoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceTexturepacker::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( MetabufLoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceTexturepacker::getFactorableType() );
        } );
#endif

#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_SET( JSONLoaderInterface, STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceTexturepacker::getFactorableType(), Helper::makeFactorableUnique<JSONLoaderResourceTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerPlugin::_finalizePlugin()
    {
        Helper::removeResourcePrototype<ResourceTexturepacker>();
        Helper::removeResourcePrototype<ResourceMultiTexturepacker>();

#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceTexturepacker::getFactorableType() );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
