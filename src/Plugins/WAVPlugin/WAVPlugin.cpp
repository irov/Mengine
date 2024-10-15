#include "WAVPlugin.h"

#include "Interface/CodecServiceInterface.h"

#include "SoundDecoderWAV.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/PluginHelper.h"

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
    bool WAVPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nowav" ) == true )
        {
            return false;
        }

        bool WAVPlugin_Available = CONFIG_VALUE( "WAVPlugin", "Available", true );

        if( WAVPlugin_Available == false )
        {
            return false;
        }

        return true;
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