#	include "VideoConverterFFMPEGToWEBM.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ApplicationInterface.h"
#	include "Interface/LogSystemInterface.h"

#	include "Logger/Logger.h"

#	include <Windows.h>

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	VideoConverterFFMPEGToWEBM::VideoConverterFFMPEGToWEBM( ServiceProviderInterface * _serviceProvider )
		: VideoConverter(_serviceProvider)
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
    //////////////////////////////////////////////////////////////////////////
    const String & VideoConverterFFMPEGToWEBM::getConvertExt() const
    {
        return m_convertExt;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToWEBM::convert()
	{
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "VideoConverterFFMPEGToWEBM::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        FileGroupInterface * fileGroup = FILE_SERVICE(m_serviceProvider)
            ->getFileGroup( m_options.pakName );

        const ConstString & pakPath = fileGroup->getPath();            

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        WString unicode_input;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, full_input.c_str(), full_input.size(), unicode_input ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "VideoConverterFFMPEGToWEBM::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
                );

            return false;
        }
                
        WString unicode_output;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, full_output.c_str(), full_output.size(), unicode_output ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "VideoConverterFFMPEGToWEBM::convert_: invalid convert output utf8 to unicode %s"
                , full_output.c_str()
                );
            
            return false;
        }
        

        WString buffer = L"ffmpeg -loglevel error -y -i \"" + unicode_input + L"\" -codec:v libvpx -quality good -cpu-used 0 -b:v 3000k -qmin 0 -qmax 42 -maxrate 3000k -bufsize 6000k -threads 2 -vf scale=-1:-1 -codec:a libvorbis -b:a 128k \"" + unicode_output + L"\"";
		
		LOGGER_INFO(m_serviceProvider)( "VideoConverterFFMPEGToWEBM:: conferting file '%ls' to '%ls'"
			, unicode_input.c_str()
			, unicode_output.c_str()
			);

        if( PLATFORM_SERVICE(m_serviceProvider)
            ->cmd( buffer ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "VideoConverterFFMPEGToWEBM::convert_: invalid convert:"
                );

            return false;
        }

        return true;
	}
}