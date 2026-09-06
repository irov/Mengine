#include "SoundConverterFFMPEGToOGG.h"

#include "Interface/PlatformServiceInterface.h"

#include "DevelopmentConverterProcess.h"

#include "Kernel/Logger.h"
#include "Kernel/PathString.h"
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

        PathString fullInput;
        fullInput.append( inputFolderPath );
        fullInput.append( inputFilePath );

        PathString fullOutput;
        fullOutput.append( outputFolderPath );
        fullOutput.append( outputFilePath );

        const Char * inputPath = fullInput.c_str();
        const Char * outputPath = fullOutput.c_str();

        const String ac = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ac" ), "2" );
        const String ar = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ar" ), "44100" );
        const String aq = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "aq" ), "" );

        ArgumentStrings arguments = {
            "-loglevel", "error",
            "-y",
            "-threads", "8",
            "-i", inputPath,
            "-map_metadata", "-1"
        };

        if( ac.empty() == false )
        {
            arguments.append( "-ac" );
            const Char * acValue = ac.c_str();
            arguments.append( acValue );
        }

        if( ar.empty() == false )
        {
            arguments.append( "-ar" );
            const Char * arValue = ar.c_str();
            arguments.append( arValue );
        }

        if( aq.empty() == false )
        {
            arguments.append( "-aq" );
            const Char * aqValue = aq.c_str();
            arguments.append( aqValue );
        }

        arguments.append( {"-acodec", "libvorbis", "-max_muxing_queue_size", "1024", outputPath} );

        LOGGER_INFO( "convert", "converting file '%s' to '%s'"
            , inputPath
            , outputPath
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
            const Char * executablePath = ffmpegPath2.c_str();

            LOGGER_ERROR( "invalid execute ffmpeg '%s'", executablePath );

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
