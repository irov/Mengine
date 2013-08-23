#	include "SoundConverterFFMPEGToOGG.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ApplicationInterface.h"
#	include "Interface/LogSystemInterface.h"

#	include "Logger/Logger.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	SoundConverterFFMPEGToOGG::SoundConverterFFMPEGToOGG( ServiceProviderInterface * _serviceProvider )
		: SoundConverter(_serviceProvider)
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	SoundConverterFFMPEGToOGG::~SoundConverterFFMPEGToOGG()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool SoundConverterFFMPEGToOGG::initialize()
	{
        m_convertExt = ".ogg";

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    const String & SoundConverterFFMPEGToOGG::getConvertExt() const
    {
        return m_convertExt;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool SoundConverterFFMPEGToOGG::convert()
	{
        FileGroupInterface * fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG::convert_: not found file group '%s'"
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
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, full_input.c_str(), full_input.size(), unicode_input ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
                );

            return false;
        }
                
        WString unicode_output;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, full_output.c_str(), full_output.size(), unicode_output ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG::convert_: invalid convert output utf8 to unicode %s"
                , full_output.c_str()
                );
            
            return false;
        }

        WString buffer = L"ffmpeg.exe -loglevel error -y -i \""  + unicode_input + L"\" -strict experimental   -acodec vorbis -aq 100 \"" + unicode_output + L"\"";
		
		LOGGER_INFO(m_serviceProvider)( "SoundDecoderConverterFFMPEGToOGG:: conferting file '%ls' to '%ls'"
			, unicode_input.c_str()
			, unicode_output.c_str()
			);

        if( PLATFORM_SERVICE(m_serviceProvider)
            ->cmd( buffer ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG::convert_: invalid convert:"
				);

			return false;
		}

        return true;
	}
}