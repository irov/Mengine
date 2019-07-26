#include "TheoraPlugin.h"

#include "Interface/CodecServiceInterface.h"

#include "TheoraVideoDecoder.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

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
    bool TheoraPlugin::_initializePlugin()
    {
        LOGGER_MESSAGE( "Video Theora Version: %s"
            , theora_version_string()
        );

        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvVideo" ) );
        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "ogv", STRINGIZE_STRING_LOCAL( "ogvVideo" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "ogva", STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TheoraPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvVideo" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );
    }
}