#include "TheoraPlugin.h"

#include "Interface/CodecServiceInterface.h"

#include "TheoraVideoDecoder.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/PluginHelper.h"

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
    bool TheoraPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "notheora" ) == true )
        {
            return false;
        }

        bool TheoraPlugin_Available = CONFIG_VALUE( "TheoraPlugin", "Available", true );

        if( TheoraPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraPlugin::_initializePlugin()
    {
        LOGGER_MESSAGE( "Theora version: %s"
            , theora_version_string()
        );

        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvVideo" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvaVideo" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "ogv" ), STRINGIZE_STRING_LOCAL( "ogvVideo" ) );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "ogva" ), STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TheoraPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvVideo" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "ogv" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "ogva" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}