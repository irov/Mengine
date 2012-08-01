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
		
		WString inputFile =  m_options.inputFileName;
		WString fileSystemPath = m_options.pakNameFullPath;
		
		WString inputFileFullPath = fileSystemPath + inputFile;
		
		int pointPos = inputFile.find_last_of(L'.');
		WString outputFile = inputFile.substr( 0 , pointPos );
		outputFile += L".ogg";

		WString outputFileFullPath = fileSystemPath + outputFile;
		if( m_fileService->existFile( m_options.pakName, inputFile ) == false )
		{
			LOGGER_ERROR(m_logService)( "Error: Can't open  input file '%S'"
				, inputFile.c_str()
				);

			return false;
		}

		if( m_fileService->existFile( m_options.pakName, outputFile ) == false )
		{
			if( this->convert_( inputFileFullPath, outputFileFullPath ) == false)
			{
				LOGGER_ERROR(m_logService)( "Error: Can't convert  sound file '%S'"
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
					LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG: Can't convert  sound file '%S'"
						, inputFileFullPath.c_str()
						);
							
					return false;
				}
			}
		}

		m_dataInfo.outputFileName = outputFile;

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool SoundConverterFFMPEGToOGG::convert_( const WString & _input, const WString & _output )
	{
		WString buffer = L"ffmpeg.exe -loglevel error -y -i \""  + _input + L"\" -strict experimental   -acodec vorbis -aq 100 \""+ _output + L"\"";
		
		LOGGER_INFO(m_logService)( "SoundDecoderConverterFFMPEGToOGG:: conferting file  '%S' to '%S'"
			, _input.c_str()
			, _output.c_str() 
			);

		int result = _wsystem( buffer.c_str() );
		
		if( result == 0 )
		{
			return true;
		}
		else
		{
			LOGGER_ERROR(m_logService)( "SoundConverterFFMPEGToOGG::convert_: %S"
				, buffer.c_str()
				);

			return false;
		}
	}
}