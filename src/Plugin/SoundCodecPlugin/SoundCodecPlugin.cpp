#include "SoundCodecPlugin.h"

#include "SoundDecoderWAV.h"

#include "Interface/StringizeInterface.h"

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
        Helper::registerDecoder<SoundDecoderWAV>( "wavSound" );

        CODEC_SERVICE()
            ->registerCodecExt( "wav", Helper::stringizeString( "wavSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundCodecPlugin::_finalize()
    {
        Helper::unregisterDecoder( "wavSound" );
    }
}