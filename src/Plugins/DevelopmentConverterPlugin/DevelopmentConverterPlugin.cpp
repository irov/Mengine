#include "DevelopmentConverterPlugin.h"

#include "Interface/ConverterServiceInterface.h"

#include "HotspotImageConverterPNGToHIT.h"
#include "ImageConverterPVRToHTF.h"
#include "ImageConverterDDSToHTF.h"
#include "ImageConverterPNGToACF.h"
#include "SoundConverterFFMPEGToOGG.h"
#include "VideoConverterFFMPEGToWEBM.h"
#include "VideoConverterFFMPEGToOGV.h"
#include "VideoConverterFFMPEGToOGVA.h"
#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "ShaderConverterTextToVSO.h"
#   include "ShaderConverterTextToVSO3.h"
#   include "ShaderConverterTextToPSO.h"
#   include "ShaderConverterTextToVSO11.h"
#   include "ShaderConverterTextToPSO11.h"
#endif

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"
#include "Plugins/DazzlePlugin/DazzleInterface.h"
#include "Plugins/MoviePlugin/Movie2Interface.h"

#include "Kernel/ArchiveDevelopmentConverter.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConverterFactory.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DevelopmentConverter, Mengine::DevelopmentConverterPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevelopmentConverterPlugin::DevelopmentConverterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevelopmentConverterPlugin::~DevelopmentConverterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevelopmentConverterPlugin::_initializePlugin()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, "not found archivator 'lz4'" );

        Helper::registerConverter<HotspotImageConverterPNGToHIT>( STRINGIZE_STRING_LOCAL( "png2hit" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<SoundConverterFFMPEGToOGG>( STRINGIZE_STRING_LOCAL( "ffmpegToOggSound" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToWEBM>( STRINGIZE_STRING_LOCAL( "ffmpegToWebM" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToOGV>( STRINGIZE_STRING_LOCAL( "ffmpegToOGV" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToOGVA>( STRINGIZE_STRING_LOCAL( "ffmpegToOGVA" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterPVRToHTF>( STRINGIZE_STRING_LOCAL( "pvr2htf" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterDDSToHTF>( STRINGIZE_STRING_LOCAL( "dds2htf" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterPNGToACF>( STRINGIZE_STRING_LOCAL( "png2acf" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ArchiveDevelopmentConverter>( STRINGIZE_STRING_LOCAL( "ptc2ptz" ), MENGINE_DOCUMENT_FACTORABLE
            , STRINGIZE_STRING_LOCAL( ".ptz" ), archivator
            , GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ), false, EAC_BEST
        );
        Helper::registerConverter<ArchiveDevelopmentConverter>( STRINGIZE_STRING_LOCAL( "dzb2dzz" ), MENGINE_DOCUMENT_FACTORABLE
            , STRINGIZE_STRING_LOCAL( ".dzz" ), archivator
            , GET_MAGIC_NUMBER( MAGIC_DZZ ), GET_MAGIC_VERSION( MAGIC_DZZ ), false, EAC_BEST
        );
        Helper::registerConverter<ArchiveDevelopmentConverter>( STRINGIZE_STRING_LOCAL( "aeb2aez" ), MENGINE_DOCUMENT_FACTORABLE
            , STRINGIZE_STRING_LOCAL( ".aez" ), archivator
            , GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), false, EAC_BEST
        );
#if defined(MENGINE_PLATFORM_WINDOWS)
        Helper::registerConverter<ShaderConverterTextToVSO>( STRINGIZE_STRING_LOCAL( "text2vso" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToVSO3>( STRINGIZE_STRING_LOCAL( "text2vso3" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToPSO>( STRINGIZE_STRING_LOCAL( "text2pso" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToVSO11>( STRINGIZE_STRING_LOCAL( "text2vso11" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToPSO11>( STRINGIZE_STRING_LOCAL( "text2pso11" ), MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentConverterPlugin::_finalizePlugin()
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2vso" ) );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2vso3" ) );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2pso" ) );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2vso11" ) );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2pso11" ) );
#endif

        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "png2hit") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToOggSound") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToWebM") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToOGV") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToOGVA") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "pvr2htf") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "dds2htf") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "png2acf") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ptc2ptz") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "dzb2dzz") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "aeb2aez") );
    }
    //////////////////////////////////////////////////////////////////////////
}
