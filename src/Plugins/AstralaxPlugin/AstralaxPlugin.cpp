#include "AstralaxPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"
#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"

#include "ResourceAstralax.h"
#include "MetabufLoaderResourceAstralax.h"

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
#include "Kernel/PrototypeHelper.h"

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
        bool AstralaxPlugin_Available = CONFIG_VALUE_BOOLEAN( "AstralaxPlugin", "Available", true );

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

        if( PROTOTYPE_SERVICE()
            ->addPrototype( Node::getFactorableType(), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), Helper::makeFactorableUnique<NodePrototypeGenerator<AstralaxEmitter, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<AstralaxEmitter, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( Resource::getFactorableType(), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceAstralax, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceAstralax, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( ResourceValidateServiceInterface, [this]()
        {
            VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceAstralax::getFactorableType(), Helper::makeFactorableUnique<ResourceAstralaxValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<ResourceAstralaxValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourceValidateServiceInterface, [this]()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceAstralax::getFactorableType() );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
        } );

        PLUGIN_SERVICE_WAIT( ResourcePrefetcherServiceInterface, [this]()
        {
            ResourcePrefetcherInterfacePtr archivePrefetcherLZ4 = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), archivePrefetcherLZ4, MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceAstralax::getFactorableType(), archivePrefetcherLZ4, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourcePrefetcherServiceInterface, [this]()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceAstralax::getFactorableType() );
        } );

        VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceAstralax::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceAstralax>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<MetabufLoaderResourceAstralax>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerConverter<ParticleConverterPTCToPTZ>( STRINGIZE_STRING_LOCAL( "ptc2ptz" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( Node::getFactorableType(), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), nullptr );

        Helper::removeNodePrototype<AstralaxEmitter>();

        PROTOTYPE_SERVICE()
            ->removePrototype( Resource::getFactorableType(), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), nullptr );

        Helper::removeResourcePrototype<ResourceAstralax>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceAstralax::getFactorableType() );
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
