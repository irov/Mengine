#include "SoundCodecPlugin.h"
#include "SoundDecoderWAV.h"

#include "Interface/CodecServiceInterface.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

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
        Helper::registerDecoder<SoundDecoderWAV>( STRINGIZE_STRING_LOCAL( "wavSound" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "wav" ), STRINGIZE_STRING_LOCAL( "wavSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundCodecPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "wavSound" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "wav" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}