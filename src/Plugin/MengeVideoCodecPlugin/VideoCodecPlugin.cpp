#include "VideoCodecPlugin.h"

//#include "VideoDecoderGVF.h"
//#include "VideoDecoderVPX.h"
#include "VideoDecoderTheora.h"

#include "Codec/DecoderFactory.h"

#include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeVideoCodec, Mengine::VideoCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoCodecPlugin::_initialize()
	{	        
        Helper::registerDecoder<VideoDecoderTheora>( "ogvVideo" );
        Helper::registerDecoder<VideoDecoderTheora>( "ogvaVideo" );

		CODEC_SERVICE()
			->registerCodecExt( "ogv", STRINGIZE_STRING_LOCAL( "ogvVideo" ) );

		CODEC_SERVICE()
			->registerCodecExt( "ogva", STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );
				
		LOGGER_WARNING("Video Theora Version: %s"
			, theora_version_string()
			);
        
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::_finalize()
	{
        Helper::unregisterDecoder( "ogvVideo" );
        Helper::unregisterDecoder( "ogvaVideo" );
	}
}