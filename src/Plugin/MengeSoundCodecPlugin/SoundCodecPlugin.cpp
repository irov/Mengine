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
        Helper::registerDecoder<SoundDecoderWAV>( m_serviceProvider, "wavSound" );
		
		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "wav", Helper::stringizeString(m_serviceProvider, "wavSound") );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundCodecPlugin::_finalize()
	{
        Helper::unregisterDecoder( m_serviceProvider, "wavSound" );
	}
}