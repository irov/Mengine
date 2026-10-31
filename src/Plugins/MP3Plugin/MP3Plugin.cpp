#include "MP3Plugin.h"

#include "Interface/CodecServiceInterface.h"

#include "SoundDecoderMP3.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MP3, Mengine::MP3Plugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MP3Plugin::MP3Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MP3Plugin::~MP3Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MP3Plugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nomp3" ) == true )
        {
            return false;
        }

        bool MP3Plugin_Available = CONFIG_VALUE_BOOLEAN( "MP3Plugin", "Available", true );

        if( MP3Plugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MP3Plugin::_initializePlugin()
    {
        const Char * mp3Version = drmp3_version_string();

        MENGINE_UNUSED( mp3Version );

        LOGGER_MESSAGE( "MP3 version: dr_mp3 %s"
            , mp3Version
        );

        Helper::registerDecoder<SoundDecoderMP3>( STRINGIZE_STRING_LOCAL( "mp3Sound" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "mp3" ), STRINGIZE_STRING_LOCAL( "mp3Sound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MP3Plugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "mp3Sound" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "mp3" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void MP3Plugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "mp3" );
    }
    //////////////////////////////////////////////////////////////////////////
}
