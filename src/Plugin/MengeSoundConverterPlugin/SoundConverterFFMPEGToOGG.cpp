#	include "SoundConverterFFMPEGToOGG.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/LogSystemInterface.h"

#	include "Logger/Logger.h"

#	include <Windows.h>

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
	bool SoundConverterFFMPEGToOGG::convert( bool & _skip )
	{
        _skip = false;

		if( m_options.inputFileName.empty() == true  || m_options.pakName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundConverterFFMPEGToOGG :: input file name or pak name is empty"
                );

			return false;
		}
		
		const FilePath & inputFile = m_options.inputFileName;
		
        FileGroupInterface * fileGroup;

		if( FILE_SERVICE(m_serviceProvider)->existFile( m_options.pakName, inputFile, &fileGroup ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG: Can't open  input file '%s'"
				, inputFile.c_str()
				);

			return false;
		}

        const FilePath & outputFile = m_options.outputFileName;

        const FilePath & fileGroupPath = fileGroup->getPath();

        FilePath fullInputPath = concatenationFilePath( m_serviceProvider, fileGroupPath, inputFile );
        FilePath fullOutputPath = concatenationFilePath( m_serviceProvider, fileGroupPath, outputFile );

		if( FILE_SERVICE(m_serviceProvider)->existFile( m_options.pakName, outputFile, NULL ) == false )
		{
			if( this->convert_( fullInputPath, fullOutputPath ) == false)
			{
				LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG: Can't convert  sound file '%s'"
					, fullInputPath.c_str()
					);
		
				return false;
			}
		}
		else
		{
			
			InputStreamInterface * oldFile =
                FILE_SERVICE(m_serviceProvider)->openInputFile( m_options.pakName, inputFile );

			if( oldFile == NULL )
			{
				return false;
			}

            time_t fileTimeInput;
			oldFile->time(fileTimeInput);
			
            oldFile->destroy();
			oldFile = NULL;
			
			InputStreamInterface * newFile = 
                FILE_SERVICE(m_serviceProvider)->openInputFile( m_options.pakName, outputFile );

			if( newFile == NULL )
			{
				return false;
			}

            time_t fileTimeOutput;
			newFile->time(fileTimeOutput);

			newFile->destroy();
			newFile = NULL;

			if( fileTimeInput > fileTimeOutput )
			{
				if( this->convert_( fullInputPath, fullOutputPath ) == false)
				{
					LOGGER_ERROR(m_serviceProvider)( "SoundDecoderConverterFFMPEGToOGG: Can't convert  sound file '%s'"
						, fullInputPath.c_str()
						);
							
					return false;
				}
			}
            else
            {
                _skip = true;
            }
		}
        
		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool SoundConverterFFMPEGToOGG::convert_( const FilePath & _input, const FilePath & _output )
	{
        WString unicode_input;

        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _input.c_str(), _input.size(), unicode_input ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG::convert_: invalid convert input utf8 to unicode %s"
                , _input.c_str()
                );

            return false;
        }
                
        WString unicode_output;

        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _output.c_str(), _output.size(), unicode_output ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG::convert_: invalid convert output utf8 to unicode %s"
                , _output.c_str()
                );
            
            return false;
        }

        WString buffer = L"ffmpeg.exe -loglevel error -y -i \""  + unicode_input + L"\" -strict experimental   -acodec vorbis -aq 100 \"" + unicode_output + L"\"";
		
		LOGGER_INFO(m_serviceProvider)( "SoundDecoderConverterFFMPEGToOGG:: conferting file '%ls' to '%ls'"
			, unicode_input.c_str()
			, unicode_output.c_str()
			);

        int err = _wsystem( buffer.c_str() );

		if( err != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "SoundConverterFFMPEGToOGG::convert_: invalid system command:\n %ls"
				, buffer.c_str()
				);

			return false;
		}

        return true;
	}
}