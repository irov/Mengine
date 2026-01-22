#include "ResourcePrefetcherService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "DataflowResourcePrefetcher.h"
#include "ArchiveResourcePrefetcher.h"
#include "ImageDecoderResourcePrefetcher.h"
#include "SoundDecoderResourcePrefetcher.h"
#include "DefaultResourcePrefetcher.h"

#include "Engine/ResourceHIT.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourcePrefetcherService, Mengine::ResourcePrefetcherService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherService::ResourcePrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherService::~ResourcePrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::_initializeService()
    {
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Default" ), Helper::makeFactorableUnique<DefaultResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ), Helper::makeFactorableUnique<DataflowResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        ArchiveResourcePrefetcherPtr archivePrefetcherLZ4 = Helper::makeFactorableUnique<ArchiveResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, "not found 'lz4' archivator" );

        archivePrefetcherLZ4->setArchivator( archivator );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ), archivePrefetcherLZ4, MENGINE_DOCUMENT_FACTORABLE );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ), Helper::makeFactorableUnique<ImageDecoderResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ), Helper::makeFactorableUnique<SoundDecoderResourcePrefetcher>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        ResourcePrefetcherInterfacePtr prefetcherImageDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );
        ResourcePrefetcherInterfacePtr prefetcherSoundDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );
        ResourcePrefetcherInterfacePtr prefetcherDefault = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Default" ) );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceImageDefault::getFactorableType(), prefetcherImageDecoder, MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceSound::getFactorableType(), prefetcherSoundDecoder, MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceHIT::getFactorableType(), prefetcherDefault, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherService::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceImageDefault::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceSound::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceHIT::getFactorableType() );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Default" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ) );
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::prefetchResource( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        //ToDo

        LOGGER_INFO( "prefetch", "prefetch resource '%s' type '%s' group '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            //, _resource->getLocale().c_str()
        );

        const ConstString & resourceType = _resource->getType();

        ResourcePrefetcherInterfacePtr prefetcher = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), resourceType );

        if( prefetcher == nullptr )
        {
            return false;
        }

        bool successful = prefetcher->prefetch( _resource, _observer );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::unfetchResource( const ResourcePtr & _resource )
    {
        //ToDo

        LOGGER_INFO( "prefetch", "unfetch resource '%s' type '%s' group '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            //, _resource->getLocale().c_str()
        );

        const ConstString & resourceType = _resource->getType();

        ResourcePrefetcherInterfacePtr prefetcher = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), resourceType );

        if( prefetcher == nullptr )
        {
            return false;
        }

        bool successful = prefetcher->unfetch( _resource );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}