#include "JSONPlugin.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "JSONScriptEmbedding.h"
#endif

#include "ResourceJSON.h"

#include "MetabufLoaderResourceJSON.h"

#include "JSONConfig.h"

#include "JSONSetting.h"
#include "JSONSettingPrototypeGenerator.h"

#include "JSONPackageLoader.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

#ifndef MENGINE_PLUGIN_JSON_SEED
#define MENGINE_PLUGIN_JSON_SEED 1
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( JSONService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( JSON, Mengine::JSONPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * my_jpp_malloc( size_t _size )
        {
            return Helper::allocateMemory( _size, "json" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void my_jpp_free( void * _ptr )
        {
            Helper::deallocateMemory( _ptr, "json" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    JSONPlugin::JSONPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONPlugin::~JSONPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONPlugin::_initializePlugin()
    {
        uint32_t JSON_Seed = CONFIG_VALUE_INTEGER( "JSONPlugin", "Seed", MENGINE_PLUGIN_JSON_SEED );

        jpp::set_object_seed( JSON_Seed );

        const char * version = jpp::get_version();

        LOGGER_MESSAGE( "jpp version: %s seed: %u"
            , version
            , JSON_Seed 
        );

        jpp::set_alloc_funcs( &Detail::my_jpp_malloc, &Detail::my_jpp_free );

        if( SERVICE_CREATE( JSONService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( JSONScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<JSONScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( JSONScriptEmbedding::getFactorableType() );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( Helper::addResourcePrototype<ResourceJSON, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Setting" ), STRINGIZE_STRING_LOCAL( "json" ), Helper::makeFactorableUnique<JSONSettingPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( LoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceJSON::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceJSON>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( LoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceJSON::getFactorableType() );
        } );

        PLUGIN_SERVICE_WAIT( ResourcePrefetcherServiceInterface, [this]()
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcherDefault = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Default" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceJSON::getFactorableType(), resourcePrefetcherDefault, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourcePrefetcherServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceJSON::getFactorableType() );
        } );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Config" ), STRINGIZE_STRING_LOCAL( "json" ), Helper::makeDefaultPrototypeGenerator<JSONConfig, 32>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        VOCABULARY_SET( PackageLoaderInterface, STRINGIZE_STRING_LOCAL( "PackageLoader" ), STRINGIZE_STRING_LOCAL( "json" ), Helper::makeFactorableUnique<JSONPackageLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Config" ), STRINGIZE_STRING_LOCAL( "json" ), nullptr );

        Helper::removeResourcePrototype<ResourceJSON>();

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Setting" ), STRINGIZE_STRING_LOCAL( "json" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "PackageLoader" ), STRINGIZE_STRING_LOCAL( "json" ) );

        SERVICE_FINALIZE( JSONService );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( JSONService );

        MENGINE_ASSERTION_ALLOCATOR( "json" );
        MENGINE_ASSERTION_ALLOCATOR( "json2" );
    }
    //////////////////////////////////////////////////////////////////////////
}
