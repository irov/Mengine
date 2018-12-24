#include "TheoraPlugin.h"

#include "TheoraVideoDecoder.h"

#include "Kernel/DecoderFactory.h"

#include "Interface/StringizeServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Theora, Mengine::TheoraPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TheoraPlugin::TheoraPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TheoraPlugin::~TheoraPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraPlugin::_initialize()
    {
        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvVideo" ) );
        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "ogv", STRINGIZE_STRING_LOCAL( "ogvVideo" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "ogva", STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        LOGGER_WARNING( "Video Theora Version: %s"
            , theora_version_string()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TheoraPlugin::_finalize()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvVideo" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );
    }
}