#include "ResourcePrefetcherPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/TextServiceInterface.h"

#include "ResourcePrefetcherServiceInterface.h"

#include "DataflowResourcePrefetcher.h"
#include "ArchiveResourcePrefetcher.h"
#include "ImageDecoderResourcePrefetcher.h"
#include "SoundDecoderResourcePrefetcher.h"
#include "DefaultResourcePrefetcher.h"
#include "ResourcePrefetcherScriptEmbedding.h"

#include "Kernel/FactorableUnique.h"

#include "pybind/pybind.hpp"

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
        SERVICE_CREATE( ResourcePrefetcherService );

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            ADD_SCRIPT_EMBEDDING( ResourcePrefetcherScriptEmbedding );
        }

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Default" ), Helper::makeFactorableUnique<DefaultResourcePrefetcher>() );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Dataflow" ), Helper::makeFactorableUnique<DataflowResourcePrefetcher>() );

        ArchiveResourcePrefetcherPtr archivePrefetcherLZ4 = Helper::makeFactorableUnique<ArchiveResourcePrefetcher>();

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        archivePrefetcherLZ4->setArchivator( archivator );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ), archivePrefetcherLZ4 );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ), Helper::makeFactorableUnique<ImageDecoderResourcePrefetcher>() );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ), Helper::makeFactorableUnique<SoundDecoderResourcePrefetcher>() );

        ResourcePrefetcherInterfacePtr prefetcherImageDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), prefetcherImageDecoder );

        ResourcePrefetcherInterfacePtr prefetcherSoundDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceSound" ), prefetcherSoundDecoder );

        ResourcePrefetcherInterfacePtr prefetcherDefault = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Default" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceHIT" ), prefetcherDefault );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( ResourcePrefetcherScriptEmbedding );
        }

        RESOURCEPREFETCHER_SERVICE()
            ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceSound" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceHIT" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Default" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );

        SERVICE_FINALIZE( Mengine::ResourcePrefetcherServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourcePrefetcherServiceInterface );
    }
}
