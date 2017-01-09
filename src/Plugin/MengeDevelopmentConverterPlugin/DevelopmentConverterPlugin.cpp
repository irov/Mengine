#	include "DevelopmentConverterPlugin.h"

#	include "Interface/StringizeInterface.h"

#	include "HotspotImageConverterPNGToHIT.h"
#	include "ParticleConverterPTCToPTZ.h"
#	include "ImageConverterPVRToHTF.h"
#	include "ImageConverterDDSToHTF.h"
#	include "ImageConverterPNGToACF.h"
#	include "SoundConverterFFMPEGToOGG.h"
#	include "VideoConverterFFMPEGToWEBM.h"
#	include "VideoConverterFFMPEGToOGV.h"
#	include "VideoConverterFFMPEGToOGVA.h"
#	include "MovieKeyConverterXMLToAEK.h"
#	include "ModelConverterMDLToMDZ.h"

#   include "Logger/Logger.h"

#   include "Codec/ConverterFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DevelopmentConverter, Menge::DevelopmentConverterPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DevelopmentConverterPlugin::DevelopmentConverterPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DevelopmentConverterPlugin::_initialize()
	{
        Helper::registerConverter<HotspotImageConverterPNGToHIT>( m_serviceProvider, "png2hit" );
        Helper::registerConverter<SoundConverterFFMPEGToOGG>( m_serviceProvider, "ffmpegToOggSound" );
        Helper::registerConverter<VideoConverterFFMPEGToWEBM>( m_serviceProvider, "ffmpegToWebM" );
        Helper::registerConverter<VideoConverterFFMPEGToOGV>( m_serviceProvider, "ffmpegToOGV" );
        Helper::registerConverter<VideoConverterFFMPEGToOGVA>( m_serviceProvider, "ffmpegToOGVA" );
        Helper::registerConverter<MovieKeyConverterXMLToAEK>( m_serviceProvider, "xmlToAekMovie" );
        Helper::registerConverter<ModelConverterMDLToMDZ>( m_serviceProvider, "mdlToMdzModel" );
        Helper::registerConverter<ParticleConverterPTCToPTZ>( m_serviceProvider, "ptc2ptz" );
        Helper::registerConverter<ImageConverterPVRToHTF>( m_serviceProvider, "pvr2htf" );
        Helper::registerConverter<ImageConverterDDSToHTF>( m_serviceProvider, "dds2htf" );
        Helper::registerConverter<ImageConverterPNGToACF>( m_serviceProvider, "png2acf" );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DevelopmentConverterPlugin::_finalize()
	{
        Helper::unregisterConverter( m_serviceProvider, "png2hit" );
        Helper::unregisterConverter( m_serviceProvider, "ffmpegToOggSound" );
        Helper::unregisterConverter( m_serviceProvider, "ffmpegToWebM" );
        Helper::unregisterConverter( m_serviceProvider, "ffmpegToOGV" );
        Helper::unregisterConverter( m_serviceProvider, "ffmpegToOGVA" );
        Helper::unregisterConverter( m_serviceProvider, "xmlToAekMovie" );
        Helper::unregisterConverter( m_serviceProvider, "mdlToMdzModel" );
        Helper::unregisterConverter( m_serviceProvider, "ptc2ptz" );
        Helper::unregisterConverter( m_serviceProvider, "pvr2htf" );
        Helper::unregisterConverter( m_serviceProvider, "dds2htf" );
        Helper::unregisterConverter( m_serviceProvider, "png2acf" );
	}
}