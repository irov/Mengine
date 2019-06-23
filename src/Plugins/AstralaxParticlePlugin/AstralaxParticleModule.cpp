#include "AstralaxParticleModule.h"

#include "AstralaxInterface.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "ResourceAstralax.h"
#include "LoaderResourceAstralax.h"
#include "AstralaxScriptEmbedding.h"


#include "AstralaxEmitter.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"
#include "ResourceAstralaxValidator.h"

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/Logger.h"

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

        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( AstralaxScriptEmbedding );

            return true;
        } );

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
            ResourcePrefetcherInterfacePtr archivePrefetcherLZ4 = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );

            RESOURCEPREFETCHER_SERVICE()
                ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceParticle" ), archivePrefetcherLZ4 );

            RESOURCEPREFETCHER_SERVICE()
                ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), archivePrefetcherLZ4 );
        }

        VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<LoaderResourceAstralax>() );
        VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<LoaderResourceAstralax>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleModule::_finalizeModule()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( AstralaxScriptEmbedding );
        }

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
            RESOURCEPREFETCHER_SERVICE()
                ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        }

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );        
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleModule::_destroy()
    {
    }
}