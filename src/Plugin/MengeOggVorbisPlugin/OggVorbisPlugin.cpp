#	include "OggVorbisPlugin.h"

#	include "SoundDecoderOGGVorbis.h"

#	include "Interface/StringizeInterface.h"

#   include "Codec/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeOggVorbis, Menge::OggVorbisPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OggVorbisPlugin::OggVorbisPlugin()       
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool OggVorbisPlugin::_initialize()
	{
        Helper::registerDecoder<SoundDecoderOGGVorbis>( m_serviceProvider, "oggSound" );
        Helper::registerDecoder<SoundDecoderOGGVorbis>( m_serviceProvider, "ogvSound" );
	
		CODEC_SERVICE(m_serviceProvider)
			->registerCodecExt( "ogg", Helper::stringizeString(m_serviceProvider, "oggSound") );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OggVorbisPlugin::_finalize()
	{
        Helper::unregisterDecoder( m_serviceProvider, "oggSound" );
        Helper::unregisterDecoder( m_serviceProvider, "ogvSound" );
	}
}