#include "VideoConverterFFMPEGToWEBM.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/Win32CreateProcess.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ParamsHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdIO.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToWEBM::VideoConverterFFMPEGToWEBM()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToWEBM::~VideoConverterFFMPEGToWEBM()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToWEBM::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".webm" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoConverterFFMPEGToWEBM::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToWEBM::convert()
    {
        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        String full_input = inputFolderPath.c_str();
        full_input += inputFilePath.c_str();

        String full_output = outputFolderPath.c_str();
        full_output += outputFilePath.c_str();

        WChar command[MENGINE_MAX_COMMAND_LENGTH + 1] = {'\0'};
        MENGINE_SWPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"-loglevel error -y -i \"%S\" -codec:v libvpx -f webm -qmin 5 -qmax 15 -threads 8 -max_muxing_queue_size 1024 \"%S\""
            , full_input.c_str()
            , full_output.c_str()
        );

        LOGGER_INFO( "convert", "converting file '%s' to '%s'\n%ls"
            , full_input.c_str()
            , full_output.c_str()
            , command
        );

        FilePath ffmpegPath = CONFIG_VALUE_FILEPATH( "Engine", "FFMPEGPath", STRINGIZE_FILEPATH_LOCAL( "ffmpeg.exe" ) );

        uint32_t exitCode;
        if( Helper::Win32CreateProcessA( ffmpegPath.c_str(), command, true, &exitCode ) == false )
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