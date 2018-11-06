#include "VideoConverterFFMPEGToWEBM.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/StringizeInterface.h"
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

        WString unicode_input;
        if( Helper::utf8ToUnicode( full_input, unicode_input ) == false )
        {
            LOGGER_ERROR( "VideoConverterFFMPEGToWEBM::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
            );

            return false;
        }

        WString unicode_output;
        if( Helper::utf8ToUnicode( full_output, unicode_output ) == false )
        {
            LOGGER_ERROR( "VideoConverterFFMPEGToWEBM::convert_: invalid convert output utf8 to unicode %s"
                , full_output.c_str()
            );

            return false;
        }

        WString buffer = L"ffmpeg -loglevel error -y -i \"" + unicode_input + L"\" -codec:v libvpx -f webm -qmin 5 -qmax 15 -threads 4 \"" + unicode_output + L"\"";

        LOGGER_WARNING( "VideoConverterFFMPEGToWEBM:: converting file '%ls' to '%ls'\n%ls"
            , unicode_input.c_str()
            , unicode_output.c_str()
            , buffer.c_str()
        );

        if( PLATFORM_SERVICE()
            ->cmd( buffer ) == false )
        {
            LOGGER_ERROR( "VideoConverterFFMPEGToWEBM::convert_: invalid convert:\n%ls"
                , buffer.c_str()
            );

            return false;
        }

        return true;
    }
}