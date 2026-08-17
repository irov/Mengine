#include "TiledMapPlugin.h"

#include "ResourceTiledMap.h"
#include "TiledMap2D.h"

#include "Interface/MetabufLoaderServiceInterface.h"

#include "MetabufLoaderResourceTiledMap.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TiledMap, Mengine::TiledMapPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TiledMapPlugin::TiledMapPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapPlugin::~TiledMapPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapPlugin::_initializePlugin()
    {
        if( Helper::addResourcePrototype<ResourceTiledMap, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<TiledMap2D, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            Helper::removeResourcePrototype<ResourceTiledMap>();

            return false;
        }

        PLUGIN_SERVICE_WAIT( MetabufLoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceTiledMap::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceTiledMap>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( MetabufLoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceTiledMap::getFactorableType() );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapPlugin::_finalizePlugin()
    {
        Helper::removeNodePrototype<TiledMap2D>();
        Helper::removeResourcePrototype<ResourceTiledMap>();
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
