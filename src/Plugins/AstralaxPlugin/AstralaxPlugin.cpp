#include "AstralaxPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"
#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"

#include "ResourceAstralax.h"
#include "MetabufLoaderResourceAstralax.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "AstralaxScriptEmbedding.h"
#endif


#include "AstralaxInterface.h"
#include "AstralaxIncluder.h"
#include "AstralaxEmitter.h"

#include "ParticleConverterPTCToPTZ.h"

#include "ResourceAstralaxValidator.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConverterFactory.h"
#include "Kernel/PluginHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AstralaxService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Astralax, Mengine::AstralaxPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxPlugin::AstralaxPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxPlugin::~AstralaxPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxPlugin::_availablePlugin() const
    {
        bool AstralaxPlugin_Available = CONFIG_VALUE( "AstralaxPlugin", "Available", true );

        return AstralaxPlugin_Available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxPlugin::_initializePlugin()
    {
        const char * version = Magic_GetVersion();

#if !defined(MENGINE_TOOLCHAIN_MINGW64) && !defined(MENGINE_PLATFORM_APPLE)
        if( StdString::strstr( version, MAGIC_API ) == nullptr )
        {
            LOGGER_ASSERTION( "Astralax Magic Particles Version: '%s' but need '%s'"
                , version
                , MAGIC_API
            );

            return false;
        }
#endif

        LOGGER_MESSAGE( "Astralax version: %s"
            , version
        );

        if( SERVICE_CREATE( AstralaxService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AstralaxScriptEmbedding" ), Helper::makeFactorableUnique<AstralaxScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AstralaxScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), Helper::makeFactorableUnique<NodePrototypeGenerator<AstralaxEmitter, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), Helper::makeFactorableUnique<NodePrototypeGenerator<AstralaxEmitter, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceAstralax, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceAstralax, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( ResourceValidateServiceInterface, [this]()
        {
            VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<ResourceAstralaxValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<ResourceAstralaxValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourceValidateServiceInterface, [this]()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
        } );

        PLUGIN_SERVICE_WAIT( ResourcePrefetcherServiceInterface, [this]()
        {
            ResourcePrefetcherInterfacePtr archivePrefetcherLZ4 = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), archivePrefetcherLZ4, MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), archivePrefetcherLZ4, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourcePrefetcherServiceInterface, [this]()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        } );

        VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<MetabufLoaderResourceAstralax>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<MetabufLoaderResourceAstralax>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerConverter<ParticleConverterPTCToPTZ>( STRINGIZE_STRING_LOCAL( "ptc2ptz" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );

        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ptc2ptz" ) );

        SERVICE_FINALIZE( AstralaxService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AstralaxService );
    }
    //////////////////////////////////////////////////////////////////////////
}
