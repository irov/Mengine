#include "DevelopmentConverterPlugin.h"

#include "Interface/StringizeInterface.h"

#include "HotspotImageConverterPNGToHIT.h"
#include "ParticleConverterPTCToPTZ.h"
#include "ImageConverterPVRToHTF.h"
#include "ImageConverterDDSToHTF.h"
#include "ImageConverterPNGToACF.h"
#include "SoundConverterFFMPEGToOGG.h"
#include "VideoConverterFFMPEGToWEBM.h"
#include "VideoConverterFFMPEGToOGV.h"
#include "VideoConverterFFMPEGToOGVA.h"
#include "MovieKeyConverterXMLToAEK.h"

#include "Kernel/Logger.h"

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
    bool DevelopmentConverterPlugin::_initialize()
    {
        Helper::registerConverter<HotspotImageConverterPNGToHIT>( "png2hit" );
        Helper::registerConverter<SoundConverterFFMPEGToOGG>( "ffmpegToOggSound" );
        Helper::registerConverter<VideoConverterFFMPEGToWEBM>( "ffmpegToWebM" );
        Helper::registerConverter<VideoConverterFFMPEGToOGV>( "ffmpegToOGV" );
        Helper::registerConverter<VideoConverterFFMPEGToOGVA>( "ffmpegToOGVA" );
        Helper::registerConverter<MovieKeyConverterXMLToAEK>( "xmlToAekMovie" );
        Helper::registerConverter<ParticleConverterPTCToPTZ>( "ptc2ptz" );
        Helper::registerConverter<ImageConverterPVRToHTF>( "pvr2htf" );
        Helper::registerConverter<ImageConverterDDSToHTF>( "dds2htf" );
        Helper::registerConverter<ImageConverterPNGToACF>( "png2acf" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentConverterPlugin::_finalize()
    {
        Helper::unregisterConverter( "png2hit" );
        Helper::unregisterConverter( "ffmpegToOggSound" );
        Helper::unregisterConverter( "ffmpegToWebM" );
        Helper::unregisterConverter( "ffmpegToOGV" );
        Helper::unregisterConverter( "ffmpegToOGVA" );
        Helper::unregisterConverter( "xmlToAekMovie" );
        Helper::unregisterConverter( "mdlToMdzModel" );
        Helper::unregisterConverter( "ptc2ptz" );
        Helper::unregisterConverter( "pvr2htf" );
        Helper::unregisterConverter( "dds2htf" );
        Helper::unregisterConverter( "png2acf" );
    }
}