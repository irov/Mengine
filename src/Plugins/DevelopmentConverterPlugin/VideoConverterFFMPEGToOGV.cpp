#include "VideoConverterFFMPEGToOGV.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/Win32CreateProcess.h"

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
        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        PathString full_input;
        full_input.append( inputFolderPath );
        full_input.append( inputFilePath );

        PathString full_output;
        full_output.append( outputFolderPath );
        full_output.append( outputFilePath );

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
            MENGINE_SNPRINTF( buffer, 256, " -vf \"scale=iw*%s:ih*%s\""
                , resize.c_str()
                , resize.c_str()
            );

            resize_cmd = buffer;
        }

        WChar command[MENGINE_MAX_COMMAND_LENGTH + 1] = {'\0'};
        MENGINE_SWPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"-loglevel error -y -threads 8 -i \"%S\"%S -vcodec libtheora -f ogg -map_metadata -1 -an%S -pix_fmt yuv420p -max_muxing_queue_size 1024 \"%S\""
            , full_input.c_str()
            , resize_cmd.c_str()
            , quality_cmd.c_str()
            , full_output.c_str()
        );

        LOGGER_MESSAGE_RELEASE( "converting file '%s' to '%s'\n%ls"
            , full_input.c_str()
            , full_output.c_str()
            , command
        );

        String ffmpeg = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ffmpeg" ), "ffmpeg.exe" );

        uint32_t exitCode;
        if( Helper::Win32CreateProcess( ffmpeg.c_str(), command, true, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%ls"
                , command
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