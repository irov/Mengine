#include "AV1Plugin.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/AllocatorSystemInterface.h"

#include "TheoraVideoDecoder.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Theora, Mengine::AV1Plugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AV1Plugin::AV1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AV1Plugin::~AV1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AV1Plugin::_initializePlugin()
    {
        LOGGER_MESSAGE( "Video AV1 Version: %u"
            , AOM_DECODER_ABI_VERSION
        );

        Helper::registerDecoder<AV1VideoDecoder>( STRINGIZE_STRING_LOCAL( "av1Video" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "av1" ), STRINGIZE_STRING_LOCAL( "av1Video" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "av1a" ), STRINGIZE_STRING_LOCAL( "av1Video" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AV1Plugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "av1Video" ) );

        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "av1" ) );
        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "av1a" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}