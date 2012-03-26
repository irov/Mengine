#	pragma once

#	include "SoundConverterFFMPEGToOGG.h"
#	include "Interface/LogSystemInterface.h"
#	include "Utils/Logger/Logger.h"
#	include <Windows.h>

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	SoundConverterFFMPEGToOGG::SoundConverterFFMPEGToOGG( ConverterServiceInterface * _service, FileServiceInterface * _fileService, LogServiceInterface * _logSystem, StringizeServiceInterface * _stringize )
		: SoundConverter( _service, _fileService, _logSystem )
		, m_stringizeService( _stringize )
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	SoundConverterFFMPEGToOGG::~SoundConverterFFMPEGToOGG()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool SoundConverterFFMPEGToOGG::initialize()
	{
		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool SoundConverterFFMPEGToOGG::convert()
	{
		if( m_options.inputFileName.empty() == true  || m_options.pakName.empty() == true )
		{
			LOGGER_ERROR(m_logService)( "SoundConverterFFMPEGToOGG :: input file name or pak name is empty " );
			return false;
		}
		
		String inputFile =  m_options.inputFileName.to_str();
		String fileSystemPath = m_options.pakNameFullPath;
		
		String inputFileFullPath = fileSystemPath + inputFile;
		
		int pointPos = inputFile.find_last_of(".");
		String outputFile = inputFile.substr( 0 , pointPos );
		outputFile += ".ogg";

		String outputFileFullPath = fileSystemPath + outputFile;
		if( m_fileService->existFile( m_options.pakName, inputFile ) == false )
		{
			LOGGER_ERROR(m_logService)( "Error: Can't open  input file '%s'"
				, inputFile.c_str()
				);

			return false;
		}

		if( m_fileService->existFile( m_options.pakName, outputFile ) == false )
		{
			if( this->convert_( inputFileFullPath, outputFileFullPath ) == false)
			{
				LOGGER_ERROR(m_logService)( "Error: Can't convert  sound file '%s'"
					, inputFileFullPath.c_str()
					);
		
				return false;
			}
		}
		else
		{
			time_t fileTimeInput;
			FileInputStreamInterface * oldFile =  m_fileService->openInputFile( m_options.pakName, inputFile );
			if( oldFile == NULL )
			{
				return false;
			}

			oldFile->time(fileTimeInput);
			oldFile->close();	
			oldFile = NULL;

			time_t fileTimeOutput;
			FileInputStreamInterface * newFile =  m_fileService->openInputFile( m_options.pakName, outputFile );
			if( newFile == NULL )
			{
				return false;
			}

			newFile->time(fileTimeOutput);
			newFile->close();	
			newFile = NULL;

			if( fileTimeInput > fileTimeOutput )
			{
				if( convert_( inputFileFullPath, outputFileFullPath ) == false)
				{
					LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG: Can't convert  sound file '%s'"
						, inputFileFullPath.c_str()
						);
							
					return false;
				}
			}
		}

		m_dataInfo.outputFileName = m_stringizeService->stringize(outputFile);
		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool SoundConverterFFMPEGToOGG::convert_( const String & input, const String & output )
	{
		String buffer; 
		buffer = "ffmpeg.exe -loglevel error -y -i \""  + input + "\" -strict experimental   -acodec vorbis -aq 100 \""+ output +"\"";
		LOGGER_INFO(m_logService)( "SoundDecoderConverterFFMPEGToOGG:: conferting file  '%s' to '%s' "
			, input.c_str()
			, output.c_str() 
			);

		int result = system(buffer.c_str());
		
		if(result == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}