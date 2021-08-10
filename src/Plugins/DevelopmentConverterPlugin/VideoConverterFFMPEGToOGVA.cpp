#include "VideoConverterFFMPEGToOGVA.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ParamsHelper.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToOGVA::VideoConverterFFMPEGToOGVA()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToOGVA::~VideoConverterFFMPEGToOGVA()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToOGVA::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".ogva" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoConverterFFMPEGToOGVA::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToOGVA::convert()
    {
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getUnknown();

        const ConstString & folderPath = m_options.fileGroup->getFolderPath();

        String full_input = folderPath.c_str();
        full_input += m_options.inputFilePath.c_str();

        String full_output = folderPath.c_str();
        full_output += m_options.outputFilePath.c_str();

        String quality = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "quality" ), "10" );

        String buffer = "-loglevel error -y -threads 4 -i \"" + full_input + "\" -vf \"split [a], pad=iw:ih*2 [b], [a] alphaextract, [b] overlay=0:h\" -vcodec libtheora -f ogg -map_metadata -1 -an -q " + quality + " -pix_fmt yuv420p -max_muxing_queue_size 1024 \"" + full_output + "\"";

        LOGGER_MESSAGE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer.c_str()
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