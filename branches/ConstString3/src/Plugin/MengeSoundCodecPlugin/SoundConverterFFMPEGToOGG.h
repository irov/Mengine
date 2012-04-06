#	pragma once
#	include "Interface/StringizeInterface.h"
#	include "SoundConverter.h"


namespace Menge
{
	class LogServiceInterface;

	class SoundConverterFFMPEGToOGG
		: public SoundConverter
	{
	public:
		SoundConverterFFMPEGToOGG( ConverterServiceInterface * _service, FileServiceInterface * _fileService, LogServiceInterface * _logSystem, StringizeServiceInterface * _stringize );
		virtual ~SoundConverterFFMPEGToOGG();

	public:
		bool initialize() override;
		bool convert() override;
	protected:
		StringizeServiceInterface * m_stringizeService;

	private:
		bool convert_( const WString & _input, const WString & _output );
	};
}	// namespace Menge
