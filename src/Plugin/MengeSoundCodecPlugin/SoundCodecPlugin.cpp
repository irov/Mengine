#	include "SoundCodecPlugin.h"

#	include "SoundDecoderWAV.h"

#	include "Interface/StringizeInterface.h"

#   include "Codec/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeSoundCodec, Menge::SoundCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundCodecPlugin::SoundCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundCodecPlugin::_initialize()
	{
        Helper::registerDecoder<SoundDecoderWAV>( "wavSound" );
		
		CODEC_SERVICE()
			->registerCodecExt( "wav", Helper::stringizeString("wavSound") );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundCodecPlugin::_finalize()
	{
        Helper::unregisterDecoder( "wavSound" );
	}
}