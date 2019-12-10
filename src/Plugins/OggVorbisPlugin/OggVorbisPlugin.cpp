#include "OggVorbisPlugin.h"

#include "Interface/CodecServiceInterface.h"

#include "SoundDecoderOGGVorbis.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

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
    bool OggVorbisPlugin::_initializePlugin()
    {
        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "oggSound" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "ogvSound" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "ogg" ), STRINGIZE_STRING_LOCAL( "oggSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OggVorbisPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "oggSound" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvSound" ) );
    }
}