#include "JSONPlugin.h"

#include "Configuration/Configurations.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "Interface/MetabufLoaderServiceInterface.h"
#endif

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"

#include "ResourceJSON.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "MetabufLoaderResourceJSON.h"
#endif

#include "JSONLoaderResourceCursorICO.h"
#include "JSONLoaderResourceCursorSystem.h"
#include "JSONLoaderResourceFile.h"
#include "JSONLoaderResourceHIT.h"
#include "JSONLoaderResourceImageData.h"
#include "JSONLoaderResourceImageDefault.h"
#include "JSONLoaderResourceImageEmpty.h"
#include "JSONLoaderResourceImageSequence.h"
#include "JSONLoaderResourceImageSolid.h"
#include "JSONLoaderResourceImageSubstract.h"
#include "JSONLoaderResourceImageSubstractRGBAndAlpha.h"
#include "JSONLoaderResourceJSON.h"
#include "JSONLoaderResourceMusic.h"
#include "JSONLoaderResourceShape.h"
#include "JSONLoaderResourceSound.h"
#include "JSONLoaderResourceWindow.h"

#include "JSONConfig.h"

#include "JSONSetting.h"
#include "JSONSettingPrototypeGenerator.h"

#include "JSONPackageContentLoader.h"
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
        static void * my_jpp_realloc( void * _ptr, size_t _size )
        {
            return Helper::reallocateMemory( _ptr, _size, "json" );
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

        jpp::set_alloc_funcs( &Detail::my_jpp_malloc, &Detail::my_jpp_realloc, &Detail::my_jpp_free );

        if( SERVICE_CREATE( JSONService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceJSON, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Setting" ), STRINGIZE_STRING_LOCAL( "json" ), Helper::makeFactorableUnique<JSONSettingPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

#if defined(MENGINE_PLUGIN_METABUF)
        PLUGIN_SERVICE_WAIT( MetabufLoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceJSON::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceJSON>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( MetabufLoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceJSON::getFactorableType() );
        } );
#endif

#define SET_JSON_LOADER(T)\
        VOCABULARY_SET( JSONLoaderInterface, STRINGIZE_STRING_LOCAL( "JSONLoader" ), T::getFactorableType(), Helper::makeFactorableUnique<MENGINE_PP_CONCATENATE(JSONLoader, T)>(MENGINE_DOCUMENT_FACTORABLE), MENGINE_DOCUMENT_FACTORABLE )

        SET_JSON_LOADER( ResourceCursorICO );
        SET_JSON_LOADER( ResourceCursorSystem );
        SET_JSON_LOADER( ResourceFile );
        SET_JSON_LOADER( ResourceHIT );
        SET_JSON_LOADER( ResourceImageData );
        SET_JSON_LOADER( ResourceImageDefault );
        SET_JSON_LOADER( ResourceImageEmpty );
        SET_JSON_LOADER( ResourceImageSequence );
        SET_JSON_LOADER( ResourceImageSolid );
        SET_JSON_LOADER( ResourceImageSubstract );
        SET_JSON_LOADER( ResourceImageSubstractRGBAndAlpha );
        SET_JSON_LOADER( ResourceJSON );
        SET_JSON_LOADER( ResourceMusic );
        SET_JSON_LOADER( ResourceShape );
        SET_JSON_LOADER( ResourceSound );
        SET_JSON_LOADER( ResourceWindow );

#undef SET_JSON_LOADER

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
        VOCABULARY_SET( PackageContentLoaderInterface, STRINGIZE_STRING_LOCAL( "PackageContentLoader" ), STRINGIZE_STRING_LOCAL( "json" ), Helper::makeFactorableUnique<JSONPackageContentLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Config" ), STRINGIZE_STRING_LOCAL( "json" ), nullptr );

        Helper::removeResourcePrototype<ResourceJSON>();

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Setting" ), STRINGIZE_STRING_LOCAL( "json" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "PackageContentLoader" ), STRINGIZE_STRING_LOCAL( "json" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "PackageLoader" ), STRINGIZE_STRING_LOCAL( "json" ) );

#define REMOVE_JSON_LOADER(T)\
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "JSONLoader" ), T::getFactorableType() )

        REMOVE_JSON_LOADER( ResourceCursorICO );
        REMOVE_JSON_LOADER( ResourceCursorSystem );
        REMOVE_JSON_LOADER( ResourceFile );
        REMOVE_JSON_LOADER( ResourceHIT );
        REMOVE_JSON_LOADER( ResourceImageData );
        REMOVE_JSON_LOADER( ResourceImageDefault );
        REMOVE_JSON_LOADER( ResourceImageEmpty );
        REMOVE_JSON_LOADER( ResourceImageSequence );
        REMOVE_JSON_LOADER( ResourceImageSolid );
        REMOVE_JSON_LOADER( ResourceImageSubstract );
        REMOVE_JSON_LOADER( ResourceImageSubstractRGBAndAlpha );
        REMOVE_JSON_LOADER( ResourceJSON );
        REMOVE_JSON_LOADER( ResourceMusic );
        REMOVE_JSON_LOADER( ResourceShape );
        REMOVE_JSON_LOADER( ResourceSound );
        REMOVE_JSON_LOADER( ResourceWindow );

#undef REMOVE_JSON_LOADER

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
