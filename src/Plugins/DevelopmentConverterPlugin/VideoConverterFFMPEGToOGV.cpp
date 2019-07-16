#include "VideoConverterFFMPEGToOGV.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToOGV::VideoConverterFFMPEGToOGV()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToOGV::~VideoConverterFFMPEGToOGV()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToOGV::_initialize()
    {
        m_convertExt = ".ogv";

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToOGV::convert()
    {
        const ConstString & pakPath = m_options.fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        String buffer = "-loglevel error -y -threads 4 -i \"" + full_input + "\" -vcodec libtheora -f ogg -map_metadata -1 -an -q 9 -pix_fmt yuv420p \"" + full_output + "\"";

        LOGGER_MESSAGE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer.c_str()
        );

        uint32_t exitCode;
        if( PLATFORM_SERVICE()
            ->createProcess( "ffmpeg.exe", buffer.c_str(), true, &exitCode ) == false )
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