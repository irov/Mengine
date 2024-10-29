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
#include "ShaderConverterTextToVSO.h"
#include "ShaderConverterTextToPSO.h"
#include "ShaderConverterTextToVSO11.h"
#include "ShaderConverterTextToPSO11.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConverterFactory.h"
#include "Kernel/PluginHelper.h"

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
        Helper::registerConverter<HotspotImageConverterPNGToHIT>( STRINGIZE_STRING_LOCAL( "png2hit" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<SoundConverterFFMPEGToOGG>( STRINGIZE_STRING_LOCAL( "ffmpegToOggSound" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToWEBM>( STRINGIZE_STRING_LOCAL( "ffmpegToWebM" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToOGV>( STRINGIZE_STRING_LOCAL( "ffmpegToOGV" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToOGVA>( STRINGIZE_STRING_LOCAL( "ffmpegToOGVA" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterPVRToHTF>( STRINGIZE_STRING_LOCAL( "pvr2htf" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterDDSToHTF>( STRINGIZE_STRING_LOCAL( "dds2htf" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterPNGToACF>( STRINGIZE_STRING_LOCAL( "png2acf" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToVSO>( STRINGIZE_STRING_LOCAL( "text2vso" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToPSO>( STRINGIZE_STRING_LOCAL( "text2pso" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToVSO11>( STRINGIZE_STRING_LOCAL( "text2vso11" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToPSO11>( STRINGIZE_STRING_LOCAL( "text2pso11" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentConverterPlugin::_finalizePlugin()
    {
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "png2hit") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToOggSound") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToWebM") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToOGV") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "ffmpegToOGVA") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "pvr2htf") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "dds2htf") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "png2acf") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2vso") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2pso") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2vso11") );
        Helper::unregisterConverter( STRINGIZE_STRING_LOCAL( "text2pso11" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}