#include "ToolEngine.h"

#include "Configuration/Configurations.h"

#include "Interface/ServiceProviderInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/ContentHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "ToolUtils/ToolLogger.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_IMAGE_CODEC_STATIC)
PLUGIN_EXPORT( ImageCodec );
#endif
#if defined(MENGINE_PLUGIN_MOVIE_STATIC)
PLUGIN_EXPORT( Movie );
#endif
#if defined(MENGINE_PLUGIN_ZIP_STATIC)
PLUGIN_EXPORT( Zip );
#endif
#if defined(MENGINE_PLUGIN_LZ4_STATIC)
PLUGIN_EXPORT( LZ4 );
#endif
#if defined(MENGINE_PLUGIN_JPEG_STATIC)
PLUGIN_EXPORT( JPEG );
#endif
#if defined(MENGINE_PLUGIN_PNG_STATIC)
PLUGIN_EXPORT( PNG );
#endif
#if defined(MENGINE_PLUGIN_PVRTC_STATIC)
PLUGIN_EXPORT( PVRTC );
#endif
#if defined(MENGINE_PLUGIN_WEBP_STATIC)
PLUGIN_EXPORT( WebP );
#endif
#if defined(MENGINE_PLUGIN_THEORA_STATIC)
PLUGIN_EXPORT( Theora );
#endif
#if defined(MENGINE_PLUGIN_OGG_VORBIS_STATIC)
PLUGIN_EXPORT( OggVorbis );
#endif
#if defined(MENGINE_PLUGIN_METABUF_STATIC)
PLUGIN_EXPORT( Metabuf );
#endif
#if defined(MENGINE_PLUGIN_DEVELOPMENTCONVERTER_STATIC)
PLUGIN_EXPORT( DevelopmentConverter );
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( DateTimeSystem );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( FileSystem );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( PlatformService );
SERVICE_EXTERN( PlatformSystem );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( TimelineService );
SERVICE_EXTERN( TimepipeService );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( VocabularyService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool ToolEngineInitialize()
    {
        ServiceProviderInterface * serviceProvider = nullptr;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );
        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorSystem, nullptr );
        SERVICE_CREATE( EnumeratorService, nullptr );
        SERVICE_CREATE( StringizeService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );
        SERVICE_CREATE( PlatformSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DateTimeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( NotificationService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FUNCTION );

        LOGGER_SERVICE()->setVerboseLevel( LM_MESSAGE );
        LOGGER_SERVICE()->registerLogger( Helper::makeFactorableUnique<ToolLogger>( MENGINE_DOCUMENT_FUNCTION ) );

        SERVICE_CREATE( ConverterService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( TimelineService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( TimepipeService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ModuleService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PrototypeService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( VocabularyService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PlatformService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FileSystem, MENGINE_DOCUMENT_FUNCTION );

#if defined(MENGINE_PLUGIN_ZIP_STATIC)
        PLUGIN_CREATE( Zip, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_LZ4_STATIC)
        PLUGIN_CREATE( LZ4, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_JPEG_STATIC)
        PLUGIN_CREATE( JPEG, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_PNG_STATIC)
        PLUGIN_CREATE( PNG, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_WEBP_STATIC)
        PLUGIN_CREATE( WebP, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_PVRTC_STATIC)
        PLUGIN_CREATE( PVRTC, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_THEORA_STATIC)
        PLUGIN_CREATE( Theora, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_OGG_VORBIS_STATIC)
        PLUGIN_CREATE( OggVorbis, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_IMAGE_CODEC_STATIC)
        PLUGIN_CREATE( ImageCodec, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_MOVIE_STATIC)
        PLUGIN_CREATE( Movie, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_METABUF_STATIC)
        PLUGIN_CREATE( Metabuf, false, MENGINE_DOCUMENT_FUNCTION );
#endif
#if defined(MENGINE_PLUGIN_DEVELOPMENTCONVERTER_STATIC)
        PLUGIN_CREATE( DevelopmentConverter, false, MENGINE_DOCUMENT_FUNCTION );
#endif

        FileGroupInterfacePtr fileGroup;

        if( FILE_SERVICE()->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), &fileGroup, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ), fileGroup, MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolEngineFinalize()
    {
        SERVICE_PROVIDER_STOP();
        PLUGIN_SERVICE()->unloadPlugins();
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );
        FILE_SERVICE()->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

        SERVICE_FINALIZE( ConfigService );
        SERVICE_FINALIZE( FileService );
        SERVICE_FINALIZE( PlatformService );
        SERVICE_FINALIZE( FileSystem );
        SERVICE_FINALIZE( PluginService );
        SERVICE_FINALIZE( ModuleService );
        SERVICE_FINALIZE( TimepipeService );
        SERVICE_FINALIZE( TimelineService );
        SERVICE_FINALIZE( MemoryService );
        SERVICE_FINALIZE( ThreadService );
        SERVICE_FINALIZE( DataService );
        SERVICE_FINALIZE( CodecService );
        SERVICE_FINALIZE( ConverterService );
        SERVICE_FINALIZE( LoggerService );
        SERVICE_FINALIZE( ArchiveService );
        SERVICE_FINALIZE( FactoryService );
        SERVICE_FINALIZE( OptionsService );
        SERVICE_FINALIZE( PrototypeService );
        SERVICE_FINALIZE( VocabularyService );
        SERVICE_FINALIZE( NotificationService );
        SERVICE_FINALIZE( EnumeratorService );
        SERVICE_FINALIZE( DateTimeSystem );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_FINALIZE( TimeSystem );
        SERVICE_FINALIZE( PlatformSystem );
    }
    //////////////////////////////////////////////////////////////////////////
}
