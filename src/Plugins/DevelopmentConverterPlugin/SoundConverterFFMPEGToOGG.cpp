#include "SoundConverterFFMPEGToOGG.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ParamsHelper.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    SoundConverterFFMPEGToOGG::SoundConverterFFMPEGToOGG()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    SoundConverterFFMPEGToOGG::~SoundConverterFFMPEGToOGG()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool SoundConverterFFMPEGToOGG::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".ogg" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundConverterFFMPEGToOGG::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool SoundConverterFFMPEGToOGG::convert()
    {
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        const ConstString & folderPath = m_options.fileGroup->getFolderPath();

        String full_input = folderPath.c_str();
        full_input += m_options.inputFilePath.c_str();

        String full_output = folderPath.c_str();
        full_output += m_options.outputFilePath.c_str();

        String ac = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ac" ), "2" );

        if( ac.empty() == false )
        {
            ac = " -ac " + ac;
        }

        String ar = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ar" ), "44100" );

        if( ar.empty() == false )
        {
            ar = " -ar " + ar;
        }

        String aq = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "aq" ), "" );

        if( aq.empty() == false )
        {
            aq = " -aq " + aq;
        }

        String buffer = "-loglevel error -y -threads 8 -i \"" + full_input + "\" -map_metadata -1" + ac + ar + aq + " -acodec libvorbis -max_muxing_queue_size 1024 \"" + full_output + "\"";

        LOGGER_MESSAGE_RELEASE( "converting file '%s' to '%s'"
            , full_input.c_str()
            , full_output.c_str()
        );

        String ffmpeg = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ffmpeg" ), "ffmpeg.exe" );

        uint32_t exitCode;
        if( win32Platform->createProcess( ffmpeg.c_str(), buffer.c_str(), true, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%s"
                , buffer.c_str()
            );

            return false;
        }

        if( exitCode != 0 )
        {
            return false;
        }

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
}