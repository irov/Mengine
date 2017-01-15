#	include "VideoCodecPlugin.h"

//#   include "VideoDecoderGVF.h"
//#   include "VideoDecoderVPX.h"
#	include "VideoDecoderTheora.h"

#   include "Codec/DecoderFactory.h"

#   include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeVideoCodec, Menge::VideoCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoCodecPlugin::_initialize()
	{	        
        Helper::registerDecoder<VideoDecoderTheora>( m_serviceProvider, "ogvVideo" );
        Helper::registerDecoder<VideoDecoderTheora>( m_serviceProvider, "ogvaVideo" );

		CODEC_SERVICE( m_serviceProvider )
			->registerCodecExt( "ogv", STRINGIZE_STRING_LOCAL( m_serviceProvider, "ogvVideo" ) );

		CODEC_SERVICE( m_serviceProvider )
			->registerCodecExt( "ogva", STRINGIZE_STRING_LOCAL( m_serviceProvider, "ogvaVideo" ) );
				
		LOGGER_WARNING( m_serviceProvider )("Video Theora Version: %s"
			, theora_version_string()
			);
        
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::_finalize()
	{
        Helper::unregisterDecoder( m_serviceProvider, "ogvVideo" );
        Helper::unregisterDecoder( m_serviceProvider, "ogvaVideo" );
	}
}