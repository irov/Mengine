#include "SoundCodecPlugin.h"

#include "SoundDecoderWAV.h"

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/DecoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( SoundCodec, Mengine::SoundCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundCodecPlugin::SoundCodecPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundCodecPlugin::_initialize()
    {
        Helper::registerDecoder<SoundDecoderWAV>( STRINGIZE_STRING_LOCAL( "wavSound" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "wav", STRINGIZE_STRING_LOCAL( "wavSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundCodecPlugin::_finalize()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "wavSound" ) );
    }
}