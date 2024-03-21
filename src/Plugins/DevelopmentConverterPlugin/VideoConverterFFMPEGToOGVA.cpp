#include "VideoConverterFFMPEGToOGVA.h"

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
        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        String full_input;
        full_input.append( inputFolderPath.c_str() );
        full_input.append( inputFilePath.c_str() );

        String full_output;
        full_output.append( outputFolderPath.c_str() );
        full_output.append( outputFilePath.c_str() );

        String quality = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "quality" ), "" );

        String quality_cmd;
        if( quality.empty() == false )
        {
            quality_cmd = " -q " + quality;
        }

        WChar command[MENGINE_MAX_COMMAND_LENGTH] = {'\0'};
        MENGINE_WNSPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"-loglevel error -y -threads 8 -i \"%S\" -vf \"split [a], pad=iw:ih*2 [b], [a] alphaextract, [b] overlay=0:h\" -vcodec libtheora -f ogg -map_metadata -1 -an%S -pix_fmt yuv420p -max_muxing_queue_size 1024 \"%S\""
            , full_input.c_str()
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