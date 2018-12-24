#include "OggVorbisPlugin.h"

//need for mingw - fseeko64
#include <stdio.h>

#include "SoundDecoderOGGVorbis.h"

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OggVorbis, Mengine::OggVorbisPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OggVorbisPlugin::OggVorbisPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OggVorbisPlugin::~OggVorbisPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OggVorbisPlugin::_initialize()
    {
        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "oggSound" ) );
        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "ogvSound" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "ogg", STRINGIZE_STRING_LOCAL( "oggSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OggVorbisPlugin::_finalize()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "oggSound" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvSound" ) );
    }
}