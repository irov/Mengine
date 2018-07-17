#include "VideoConverterFFMPEGToOGV.h"

#include "Interface/UnicodeInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/WindowsLayerInterface.h"

#include "Kernel/Logger.h"

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
        m_convertExt = ".ogv";

        return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToOGV::convert()
	{
        const ConstString & pakPath = m_options.fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        WString unicode_input;
        if( Helper::utf8ToUnicode( full_input, unicode_input ) == false )
        {
            LOGGER_ERROR("VideoConverterFFMPEGToOGV::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
                );

            return false;
        }
                
        WString unicode_output;
        if( Helper::utf8ToUnicode( full_output, unicode_output ) == false )
        {
            LOGGER_ERROR("VideoConverterFFMPEGToOGV::convert_: invalid convert output utf8 to unicode %s"
                , full_output.c_str()
                );
            
            return false;
        }
        
        WString buffer = L"ffmpeg -loglevel error -y -threads 4 -i \"" + unicode_input + L"\" -vcodec libtheora -f ogg -map_metadata -1 -an -q 9 -pix_fmt yuv420p \"" + unicode_output + L"\"";
		
		LOGGER_WARNING( "VideoConverterFFMPEGToOGV:: converting file '%ls' to '%ls'\n%ls"
			, unicode_input.c_str()
			, unicode_output.c_str()
            , buffer.c_str()
			);

        if( WINDOWSLAYER_SERVICE()
            ->cmd( buffer ) == false )
        {
            LOGGER_ERROR("VideoConverterFFMPEGToOGV::convert_: invalid convert:\n%ls"
                , buffer.c_str()
                );

            return false;
        }

        return true;
	}
}