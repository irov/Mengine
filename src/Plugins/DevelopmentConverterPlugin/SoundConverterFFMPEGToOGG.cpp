#include "SoundConverterFFMPEGToOGG.h"

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

        WChar command[MENGINE_MAX_COMMAND_LENGTH + 1] = {'\0'};
        MENGINE_SWPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"-loglevel error -y -threads 8 -i \"%S\" -map_metadata -1%S%S%S -acodec libvorbis -max_muxing_queue_size 1024 \"%S\""
            , full_input.c_str()
            , ac.c_str()
            , ar.c_str()
            , aq.c_str()
            , full_output.c_str()
        );

        LOGGER_INFO( "convert", "converting file '%s' to '%s'"
            , full_input.c_str()
            , full_output.c_str()
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