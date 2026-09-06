#include "VideoConverterFFMPEGToOGV.h"

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

        PathString fullInput;
        fullInput.append( inputFolderPath );
        fullInput.append( inputFilePath );

        PathString fullOutput;
        fullOutput.append( outputFolderPath );
        fullOutput.append( outputFilePath );

        const Char * inputPath = fullInput.c_str();
        const Char * outputPath = fullOutput.c_str();

        String quality = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "quality" ), "" );
        String resize = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "resize" ), "None" );
        
        ArgumentStrings arguments = {
            "-loglevel", "error",
            "-y",
            "-threads", "8",
            "-i", inputPath
        };

        if( resize != "None" )
        {
            PathString filter;
            filter.append( "scale=iw*" );
            filter.append( resize );
            filter.append( ":ih*" );
            filter.append( resize );

            arguments.append( "-vf" );
            const Char * filterValue = filter.c_str();
            arguments.append( filterValue );
        }

        arguments.append( {"-vcodec", "libtheora", "-f", "ogg", "-map_metadata", "-1", "-an"} );

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
