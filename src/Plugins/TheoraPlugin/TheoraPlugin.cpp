#include "TheoraPlugin.h"

#include "TheoraVideoDecoder.h"

#include "Kernel/DecoderFactory.h"

#include "Interface/StringizeServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Theora, Mengine::TheoraPlugin);
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
        Helper::registerDecoder<TheoraVideoDecoder>( "ogvVideo" );
        Helper::registerDecoder<TheoraVideoDecoder>( "ogvaVideo" );

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
        Helper::unregisterDecoder( "ogvVideo" );
        Helper::unregisterDecoder( "ogvaVideo" );
    }
}