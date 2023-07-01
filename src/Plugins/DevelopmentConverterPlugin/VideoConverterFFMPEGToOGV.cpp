#include "VideoConverterFFMPEGToOGV.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/Win32PlatformServiceExtensionInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ParamsHelper.h"

#include "Config/StdIO.h"

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
        m_convertExt = STRINGIZE_STRING_LOCAL( ".ogv" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoConverterFFMPEGToOGV::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToOGV::convert()
    {
        Win32PlatformServiceExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        const ConstString & folderPath = m_options.fileGroup->getFolderPath();

        String full_input = folderPath.c_str();
        full_input += m_options.inputFilePath.c_str();

        String full_output = folderPath.c_str();
        full_output += m_options.outputFilePath.c_str();

        String quality = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "quality" ), "" );
        String resize = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "resize" ), "None" );
        
        String quality_cmd;
        if( quality.empty() == false )
        {
            quality_cmd = " -q " + quality;
        }

        String resize_cmd;
        if( resize != "None" )
        {
            Char buffer[256] = {"/0"};
            MENGINE_SNPRINTF( buffer, 256, "-vf \"scale=iw*%s:ih*%s\""
                , resize.c_str()
                , resize.c_str()
            );

            resize_cmd = buffer;
        }

        String buffer = "-loglevel error -y -threads 8 -i \"" + full_input + "\"" + resize_cmd + " -vcodec libtheora -f ogg -map_metadata -1 -an" + quality_cmd + " -pix_fmt yuv420p -max_muxing_queue_size 1024 \"" + full_output + "\"";

        LOGGER_MESSAGE_RELEASE( "converting file '%s' to '%s'\n%s"
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