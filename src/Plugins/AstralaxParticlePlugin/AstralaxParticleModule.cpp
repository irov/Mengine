#include "AstralaxParticleModule.h"

#include "AstralaxInterface.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "ResourceAstralax.h"
#include "LoaderResourceAstralax.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "AstralaxScriptEmbedding.h"
#endif

#include "AstralaxEmitter.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"
#include "ResourceAstralaxValidator.h"

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticleModule::AstralaxParticleModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticleModule::~AstralaxParticleModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxParticleModule::_initializeModule()
    {
        LOGGER_INFO( "Initializing Particle System ..." );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "AstralaxScriptEmbedding" ), Helper::makeFactorableUnique<AstralaxScriptEmbedding>() );
        } );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "AstralaxScriptEmbedding" ) );
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), Helper::makeFactorableUnique<NodePrototypeGenerator<AstralaxEmitter, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), Helper::makeFactorableUnique<NodePrototypeGenerator<AstralaxEmitter, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceAstralax, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceAstralax, 128>>() ) == false )
        {
            return false;
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<ResourceAstralaxValidator>() );
            VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<ResourceAstralaxValidator>() );
        }

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            ResourcePrefetcherInterfacePtr archivePrefetcherLZ4 = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), archivePrefetcherLZ4 );
            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), archivePrefetcherLZ4 );
        }

        VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<LoaderResourceAstralax>() );
        VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<LoaderResourceAstralax>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleModule::_finalizeModule()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
        }

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        }

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
    }
}