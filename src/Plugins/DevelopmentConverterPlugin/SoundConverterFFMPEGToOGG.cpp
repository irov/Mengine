#include "SoundConverterFFMPEGToOGG.h"
#include "DevelopmentConverterProcess.h"

#include "Interface/PlatformServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ParamsHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConfigHelper.h"

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

        const String ac = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ac" ), "2" );
        const String ar = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ar" ), "44100" );
        const String aq = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "aq" ), "" );

        std::vector<String> arguments = {
            "-loglevel", "error",
            "-y",
            "-threads", "8",
            "-i", full_input,
            "-map_metadata", "-1"
        };

        if( ac.empty() == false )
        {
            arguments.emplace_back( "-ac" );
            arguments.emplace_back( ac );
        }

        if( ar.empty() == false )
        {
            arguments.emplace_back( "-ar" );
            arguments.emplace_back( ar );
        }

        if( aq.empty() == false )
        {
            arguments.emplace_back( "-aq" );
            arguments.emplace_back( aq );
        }

        arguments.insert( arguments.end(), {"-acodec", "libvorbis", "-max_muxing_queue_size", "1024", full_output} );

        LOGGER_INFO( "convert", "converting file '%s' to '%s'"
            , full_input.c_str()
            , full_output.c_str()
        );

#if defined(MENGINE_PLATFORM_WINDOWS)
        FilePath ffmpegPath = CONFIG_VALUE_FILEPATH( "Engine", "FFMPEGPath", STRINGIZE_FILEPATH_LOCAL( "ffmpeg.exe" ) );
#else
        FilePath ffmpegPath = CONFIG_VALUE_FILEPATH( "Engine", "FFMPEGPath", STRINGIZE_FILEPATH_LOCAL( "ffmpeg" ) );
#endif

        FilePath ffmpegPath2 = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ffmpeg" ), ffmpegPath );

        uint32_t exitCode;
        if( Helper::executeDevelopmentConverterProcess( ffmpegPath2, arguments, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid execute ffmpeg '%s'", ffmpegPath2.c_str() );

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
