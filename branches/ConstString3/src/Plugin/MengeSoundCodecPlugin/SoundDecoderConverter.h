#	pragma once
#	include "Interface/SoundCodecInterface.h"
#	include "Interface/FileSystemInterface.h"
namespace Menge
{
	class SoundDecoderConverter:
		 public SoundDecoderInterface
	{
	public:
		SoundDecoderConverter( CodecServiceInterface * _service, InputStreamInterface * _stream, FileServiceInterface * _fileService );
		virtual ~SoundDecoderConverter();
	public:
		virtual const SoundCodecDataInfo * getCodecDataInfo() const override;
		virtual void setOptions( CodecOptions * _options ) override;
	
	protected:
		virtual void _invalidate();
		virtual InputStreamInterface * getStream() const;
	
	protected:
		virtual void destroy();
	
	protected:
		SoundCodecOptions m_options;
		SoundCodecDataInfo m_dataInfo;
		CodecServiceInterface * m_service;
		InputStreamInterface * m_stream;
		FileServiceInterface * m_fileService;
	};
}
