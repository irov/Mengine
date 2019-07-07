#include "VideoConverterFFMPEGToWEBM.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

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
        m_convertExt = ".webm";

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToWEBM::convert()
    {
        const ConstString & pakPath = m_options.fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        //WString unicode_input;
        //if( Helper::utf8ToUnicode( full_input, unicode_input ) == false )
        //{
        //    LOGGER_ERROR( "VideoConverterFFMPEGToWEBM::convert_: invalid convert input utf8 to unicode %s"
        //        , full_input.c_str()
        //    );

        //    return false;
        //}

        //WString unicode_output;
        //if( Helper::utf8ToUnicode( full_output, unicode_output ) == false )
        //{
        //    LOGGER_ERROR( "VideoConverterFFMPEGToWEBM::convert_: invalid convert output utf8 to unicode %s"
        //        , full_output.c_str()
        //    );

        //    return false;
        //}

        String buffer = " -loglevel error -y -i \"" + full_input + "\" -codec:v libvpx -f webm -qmin 5 -qmax 15 -threads 4 \"" + full_output + "\"";

        LOGGER_MESSAGE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer.c_str()
        );

        uint32_t exitCode;
        if( PLATFORM_SERVICE()
            ->createProcess( "ffmpeg.exe", buffer.c_str(), &exitCode ) == false )
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
}