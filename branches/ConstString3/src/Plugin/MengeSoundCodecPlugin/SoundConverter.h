#	pragma once

#	include "Interface/ConverterInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class SoundConverter:
		 public ConverterInterface
	{
	public:
		SoundConverter( ConverterServiceInterface * _service, FileServiceInterface * _fileService, LogServiceInterface * _logService );
		virtual ~SoundConverter();
	public:
		virtual const ConverterDataInfo * getConverterDataInfo() const override;
		virtual void setOptions( ConverterOptions * _options ) override;
		virtual void destroy() override;

	protected:
		ConverterOptions m_options;
		ConverterDataInfo m_dataInfo;
		LogServiceInterface * m_logService;
		ConverterServiceInterface * m_service;
		FileServiceInterface * m_fileService;
	};
}
