#include "VideoConverterFFMPEGToOGVA.h"

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

        PathString fullInput;
        fullInput.append( inputFolderPath );
        fullInput.append( inputFilePath );

        PathString fullOutput;
        fullOutput.append( outputFolderPath );
        fullOutput.append( outputFilePath );

        const Char * inputPath = fullInput.c_str();
        const Char * outputPath = fullOutput.c_str();

        String quality = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "quality" ), "" );

        ArgumentStrings arguments = {
            "-loglevel", "error",
            "-y",
            "-threads", "8",
            "-i", inputPath,
            "-vf", "split [a], pad=iw:ih*2 [b], [a] alphaextract, [b] overlay=0:h",
            "-vcodec", "libtheora",
            "-f", "ogg",
            "-map_metadata", "-1",
            "-an"
        };

        if( quality.empty() == false )
        {
            arguments.append( "-q" );
            const Char * qualityValue = quality.c_str();
            arguments.append( qualityValue );
        }

        arguments.append( {"-pix_fmt", "yuv420p", "-max_muxing_queue_size", "1024", outputPath} );

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
