#	include "OggVorbisPlugin.h"

//need for mingw - fseeko64
#   include <stdio.h>

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
        Helper::registerDecoder<SoundDecoderOGGVorbis>( "oggSound" );
        Helper::registerDecoder<SoundDecoderOGGVorbis>( "ogvSound" );
	
		CODEC_SERVICE()
			->registerCodecExt( "ogg", Helper::stringizeString("oggSound") );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OggVorbisPlugin::_finalize()
	{
        Helper::unregisterDecoder( "oggSound" );
        Helper::unregisterDecoder( "ogvSound" );
	}
}