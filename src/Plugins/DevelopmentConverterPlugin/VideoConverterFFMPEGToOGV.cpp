#include "VideoConverterFFMPEGToOGV.h"
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
        
        std::vector<String> arguments = {
            "-loglevel", "error",
            "-y",
            "-threads", "8",
            "-i", String( full_input.c_str() )
        };

        if( resize != "None" )
        {
            arguments.emplace_back( "-vf" );
            arguments.emplace_back( "scale=iw*" + resize + ":ih*" + resize );
        }

        arguments.insert( arguments.end(), {"-vcodec", "libtheora", "-f", "ogg", "-map_metadata", "-1", "-an"} );

        if( quality.empty() == false )
        {
            arguments.emplace_back( "-q" );
            arguments.emplace_back( quality );
        }

        arguments.insert( arguments.end(), {"-pix_fmt", "yuv420p", "-max_muxing_queue_size", "1024", String( full_output.c_str() )} );

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
