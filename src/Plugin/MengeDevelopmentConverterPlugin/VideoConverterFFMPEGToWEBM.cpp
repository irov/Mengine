#	include "VideoConverterFFMPEGToWEBM.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#	include "Logger/Logger.h"

namespace Menge
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
	bool VideoConverterFFMPEGToWEBM::initialize()
	{
        m_convertExt = ".webm";

        return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToWEBM::convert()
	{
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        WString unicode_input;
        if( Helper::utf8ToUnicode( m_serviceProvider, full_input, unicode_input ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
                );

            return false;
        }
                
        WString unicode_output;
        if( Helper::utf8ToUnicode( m_serviceProvider, full_output, unicode_output ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: invalid convert output utf8 to unicode %s"
                , full_output.c_str()
                );
            
            return false;
        }
        
        WString buffer = L"ffmpeg -loglevel error -y -i \"" + unicode_input + L"\" -codec:v libvpx -f webm -qmin 5 -qmax 15 -threads 4 \"" + unicode_output + L"\"";
		
		LOGGER_WARNING(m_serviceProvider)( "VideoConverterFFMPEGToWEBM:: converting file '%ls' to '%ls'\n%ls"
			, unicode_input.c_str()
			, unicode_output.c_str()
            , buffer.c_str()
			);

        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->cmd( buffer ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: invalid convert:\n%ls"
                , buffer.c_str()
                );

            return false;
        }

        return true;
	}
}