#include "WAVPlugin.h"

#include "Interface/CodecServiceInterface.h"

#include "SoundDecoderWAV.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( WAV, Mengine::WAVPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    WAVPlugin::WAVPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WAVPlugin::~WAVPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WAVPlugin::_initializePlugin()
    {
        Helper::registerDecoder<SoundDecoderWAV>( STRINGIZE_STRING_LOCAL( "wavSound" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "wav" ), STRINGIZE_STRING_LOCAL( "wavSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WAVPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "wavSound" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "wav" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}