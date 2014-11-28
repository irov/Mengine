#	include "VideoConverterFFMPEGToOGVA.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#	include "Logger/Logger.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
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
	bool VideoConverterFFMPEGToOGVA::initialize()
	{
        m_convertExt = ".ogv";

        return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToOGVA::convert()
	{
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToOGVA::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        WString unicode_input;
        if( Helper::utf8ToUnicode( m_serviceProvider, full_input, unicode_input ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToOGVA::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
                );

            return false;
        }
                
        WString unicode_output;
        if( Helper::utf8ToUnicode( m_serviceProvider, full_output, unicode_output ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToOGVA::convert_: invalid convert output utf8 to unicode %s"
                , full_output.c_str()
                );
            
            return false;
        }
        
        WString buffer = L"ffmpeg -loglevel error -y -threads 4 -i \"" + unicode_input + L"\" -vf \"split [a], pad=iw:ih*2 [b], [a] alphaextract, [b] overlay=0:h\" -q 10 \"" + unicode_output + L"\"";
		
		LOGGER_WARNING(m_serviceProvider)( "VideoConverterFFMPEGToOGVA:: converting file '%ls' to '%ls'\n%ls"
			, unicode_input.c_str()
			, unicode_output.c_str()
            , buffer.c_str()
			);

        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->cmd( buffer ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToOGVA::convert_: invalid convert:\n%ls"
                , buffer.c_str()
                );

            return false;
        }

        return true;
	}
}