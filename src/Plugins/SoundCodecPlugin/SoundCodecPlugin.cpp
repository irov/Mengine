#include "SoundCodecPlugin.h"
#include "SoundDecoderWAV.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

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
    SoundCodecPlugin::~SoundCodecPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundCodecPlugin::_initializePlugin()
    {
        Helper::registerDecoder<SoundDecoderWAV>( STRINGIZE_STRING_LOCAL( "wavSound" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "wav", STRINGIZE_STRING_LOCAL( "wavSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundCodecPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "wavSound" ) );
    }
}