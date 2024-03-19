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
        Helper::registerConverter<HotspotImageConverterPNGToHIT>( "png2hit", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<SoundConverterFFMPEGToOGG>( "ffmpegToOggSound", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToWEBM>( "ffmpegToWebM", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToOGV>( "ffmpegToOGV", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<VideoConverterFFMPEGToOGVA>( "ffmpegToOGVA", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterPVRToHTF>( "pvr2htf", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterDDSToHTF>( "dds2htf", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ImageConverterPNGToACF>( "png2acf", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToVSO>( "text2vso", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToPSO>( "text2pso", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToVSO11>( "text2vso11", MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerConverter<ShaderConverterTextToPSO11>( "text2pso11", MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentConverterPlugin::_finalizePlugin()
    {
        Helper::unregisterConverter( "png2hit" );
        Helper::unregisterConverter( "ffmpegToOggSound" );
        Helper::unregisterConverter( "ffmpegToWebM" );
        Helper::unregisterConverter( "ffmpegToOGV" );
        Helper::unregisterConverter( "ffmpegToOGVA" );
        Helper::unregisterConverter( "xmlToAekMovie" );
        Helper::unregisterConverter( "pvr2htf" );
        Helper::unregisterConverter( "dds2htf" );
        Helper::unregisterConverter( "png2acf" );
        Helper::unregisterConverter( "text2vso" );
        Helper::unregisterConverter( "text2pso" );
        Helper::unregisterConverter( "text2vso11" );
        Helper::unregisterConverter( "text2pso11" );
    }
    //////////////////////////////////////////////////////////////////////////
}