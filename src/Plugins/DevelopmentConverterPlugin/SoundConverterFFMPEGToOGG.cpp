#include "SoundConverterFFMPEGToOGG.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

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
            ->getUnknown();

        const ConstString & folderPath = m_options.fileGroup->getFolderPath();

        String full_input = folderPath.c_str();
        full_input += m_options.inputFilePath.c_str();

        String full_output = folderPath.c_str();
        full_output += m_options.outputFilePath.c_str();

        String buffer = "-loglevel error -y -threads 4 -i \"" + full_input + "\" -map_metadata -1 -ac 2 -ar 44100 -acodec libvorbis -aq 100 \"" + full_output + "\" -max_muxing_queue_size 1024";

        LOGGER_MESSAGE( "converting file '%s' to '%s'"
            , full_input.c_str()
            , full_output.c_str()
        );

        uint32_t exitCode;
        if( win32Platform->createProcess( "ffmpeg.exe", buffer.c_str(), true, &exitCode ) == false )
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