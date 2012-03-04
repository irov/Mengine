//#	include "Utils/Logger/Logger.h"
//#	include "SoundDecoderConverterFFMPEGToOGG.h"
//#	include "Utils/Core/File.h"
//#	include <Windows.h>
////#	include "Menge/CodecEngine.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	SoundDecoderConverterFFMPEGToOGG::SoundDecoderConverterFFMPEGToOGG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogServiceInterface * _logService, FileServiceInterface * _fileService )
//		: SoundDecoderConverter(_service, _stream, _fileService)
//		, m_logService(_logService)
//		, m_decoder(NULL)
//		, m_convertedStream(NULL)
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	SoundDecoderConverterFFMPEGToOGG::~SoundDecoderConverterFFMPEGToOGG()
//	{
//		clear_();	
//	}
//	//////////////////////////////////////////////////////////////////////////
//	const SoundCodecDataInfo* SoundDecoderConverterFFMPEGToOGG::getCodecDataInfo() const
//	{
//		if (m_dataInfo.size == 1)
//		{
//			LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG ::ERROR You must set options to converter before asking codec info  " );
//		}
//	
//		return &m_dataInfo;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool SoundDecoderConverterFFMPEGToOGG::isValid() const
//	{
//		return m_isValid;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool SoundDecoderConverterFFMPEGToOGG::initialize()
//	{
//		m_dataInfo.channels =  0;
//		m_dataInfo.frequency = 0;
//		m_dataInfo.time_total_secs = 0;
//		m_dataInfo.size = 1;
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool  SoundDecoderConverterFFMPEGToOGG::seek( float _timing )
//	{
//		if( m_decoder == NULL )
//		{
//			LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG seek:: ERROR  bad initalisation status  " );
//			return false;
//		}
//		bool result = m_decoder->seek( _timing );
//		return result;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float  SoundDecoderConverterFFMPEGToOGG::timeTell()
//	{
//		if( m_decoder == NULL )
//		{
//			LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG timeTell:: ERROR  bad initalisation status  " );
//			return 0.0f;
//		}
//
//		float result = m_decoder->timeTell();
//		return result;
//	}
//	////////////////////////////////////////////////////////////////////////// 
//	unsigned int SoundDecoderConverterFFMPEGToOGG::decode( unsigned char* _buffer, unsigned int _bufferSize )
//	{
//		if( m_isValid == false)
//		{
//			LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG :: ERROR  bad initalisation status  " );
//			return 0;
//		}
//
//		if(m_dataInfo.size <= 1)
//		{
//			LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG :: ERROR  stream is not initialised  " );
//			return 0;
//		}
//		if( _bufferSize != m_dataInfo.size )
//		{
//			LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG :: ERROR  buffer size not equal real stream size  " );
//			return 0;
//		}
//
//		int decoded = m_decoder->decode( _buffer, _bufferSize );
//		return decoded;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void SoundDecoderConverterFFMPEGToOGG::setOptions( CodecOptions * _options )
//	{
//		m_options = *static_cast<SoundCodecOptions*>(_options);
//
//		this->_invalidate();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void SoundDecoderConverterFFMPEGToOGG::_invalidate()
//	{
//		m_isValid = false;
//		
//		if( m_options.inputFileFullPath.empty() == true)
//		{
//			LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG :: input file name is empty " );
//			return;
//		}
//				
//		String inputFileFullPath = m_options.inputFileFullPath;
//
//		int pointPos = inputFileFullPath.find_last_of(".");
//		
//		String outputFile = inputFileFullPath.substr( 0 , pointPos );
//		outputFile += ".ogg";
//
//		if( m_fileService->existFile( outputFile ) == false )
//		{
//			if( this->convert_( inputFileFullPath, outputFile ) == false)
//			{
//				LOGGER_ERROR(m_logService)( "Error: Can't convert  sound file '%s'"
//					, inputFileFullPath.c_str()
//					);
//
//				return;
//			}
//		}
//		else
//		{
//			time_t fileTimeInput;
//			
//			m_stream->time(fileTimeInput);
//			
//			m_convertedStream = m_fileService->openInputStream( outputFile );
//
//			time_t fileTimeOutput;
//			m_convertedStream->time(fileTimeOutput);
//
//			m_fileService->closeInputStream(m_convertedStream);
//			
//			if( fileTimeInput > fileTimeOutput )
//			{
//				if( convert_( inputFileFullPath, outputFile ) == false)
//				{
//					LOGGER_ERROR(m_logService)( "SoundDecoderConverterFFMPEGToOGG: Can't convert  sound file '%s'"
//						, inputFileFullPath.c_str()
//						);
//					
//					return;
//				}
//			}
//		}
//
//		m_convertedStream = m_fileService->openInputStream( outputFile );
//		//m_convertedStream = fileSystem->openInputStream( String("42.ogg") );
//		if( m_convertedStream == NULL )
//		{
//			LOGGER_ERROR(m_logService)( "Error: Cannot open converted  ogg file '%s'"
//				, outputFile.c_str()
//				);
//		}
//		
//		m_decoder = m_service->createDecoderT<SoundDecoderInterface>( "oggSound", m_convertedStream );
//		
//		if( m_decoder == NULL )
//		{
//			LOGGER_ERROR(m_logService)( "Error: Can't create sound decoder for converted file '%s'"
//				, outputFile.c_str() 
//				);
//			fileSystem->closeInputStream(m_convertedStream);
//			return;
//		}
//		
//		const SoundCodecDataInfo* dataInfo = m_decoder->getCodecDataInfo();
//		m_dataInfo.channels =  dataInfo->channels;
//		m_dataInfo.frequency = dataInfo->frequency;
//		m_dataInfo.time_total_secs = dataInfo->time_total_secs;
//		m_dataInfo.size =  dataInfo->size;
//		m_isValid = true;
//	}
//	/////////////////////////////////////////////////////////////////////////
//	bool SoundDecoderConverterFFMPEGToOGG::convert_( const String & input, const String & output )
//	{
//		String buffer; 
//		buffer = "ffmpeg.exe -loglevel error -y -i \""  + input + "\" -strict experimental   -acodec vorbis -aq 100 \""+ output +"\"";
//		LOGGER_INFO(m_logService)( "SoundDecoderConverterFFMPEGToOGG:: conferting file  '%s' to '%s' "
//			, input.c_str()
//			, output.c_str() 
//			);
//		int result = system(buffer.c_str());
//		if(result == 0)
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	
//	void SoundDecoderConverterFFMPEGToOGG::clear_()
//	{
//		if(m_decoder != NULL)
//		{
//			m_decoder->destroy();
//			m_decoder = NULL;
//		}
//		if( m_convertedStream != NULL)
//		{
//			m_fileService->getFileSystemInterface()->closeInputStream(m_convertedStream);
//			m_convertedStream = NULL;
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//}