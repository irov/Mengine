#include "ResourcePrefetcherPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "ResourcePrefetcherServiceInterface.h"

#include "DataflowResourcePrefetcher.h"
#include "ArchiveResourcePrefetcher.h"
#include "ImageDecoderResourcePrefetcher.h"
#include "SoundDecoderResourcePrefetcher.h"
#include "DefaultResourcePrefetcher.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "ResourcePrefetcherScriptEmbedding.h"
#endif

#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ResourcePrefetcherService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourcePrefetcher, Mengine::ResourcePrefetcherPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherPlugin::ResourcePrefetcherPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherPlugin::~ResourcePrefetcherPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherPlugin::_initializePlugin()
    {
        SERVICE_CREATE( ResourcePrefetcherService, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherScriptEmbedding" ), Helper::makeFactorableUnique<ResourcePrefetcherScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Default" ), Helper::makeFactorableUnique<DefaultResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ), Helper::makeFactorableUnique<DataflowResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        ArchiveResourcePrefetcherPtr archivePrefetcherLZ4 = Helper::makeFactorableUnique<ArchiveResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        archivePrefetcherLZ4->setArchivator( archivator );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ), archivePrefetcherLZ4, MENGINE_DOCUMENT_FACTORABLE );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ), Helper::makeFactorableUnique<ImageDecoderResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ), Helper::makeFactorableUnique<SoundDecoderResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        ResourcePrefetcherInterfacePtr prefetcherImageDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );
        ResourcePrefetcherInterfacePtr prefetcherSoundDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );
        ResourcePrefetcherInterfacePtr prefetcherDefault = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Default" ) );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), prefetcherImageDecoder, MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ), prefetcherSoundDecoder, MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceHIT" ), prefetcherDefault, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceHIT" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Default" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );

        SERVICE_FINALIZE( ResourcePrefetcherService );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( ResourcePrefetcherService );
    }
    //////////////////////////////////////////////////////////////////////////
}
