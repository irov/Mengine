#include "SoundConverterFFMPEGToOGG.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

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
        m_convertExt = ".ogg";

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool SoundConverterFFMPEGToOGG::convert()
    {
        const ConstString & pakPath = m_options.fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        String buffer = " -loglevel error -y -threads 4 -i \"" + full_input + "\" -map_metadata -1 -ac 2 -ar 44100 -acodec libvorbis -aq 100 \"" + full_output + "\"";

        LOGGER_MESSAGE( "converting file '%s' to '%s'"
            , full_input.c_str()
            , full_output.c_str()
        );

        uint32_t exitCode;
        if( PLATFORM_SERVICE()
            ->createProcess( "ffmpeg.exe", buffer.c_str(), &exitCode ) == false )
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
}