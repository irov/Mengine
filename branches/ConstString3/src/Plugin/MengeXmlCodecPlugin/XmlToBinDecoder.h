#	pragma once

#	include "Interface/XmlCodecInterface.h"
#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

namespace Menge
{
	class InputStreamInterface;

	class Xml2BinDecoder
		: public XmlDecoderInterface
	{
	public:
		Xml2BinDecoder( LogServiceInterface * _logService, UnicodeServiceInterface * _unicodeService );
	
	public:
		void setOptions( CodecOptions * _options ) override;

	public:
		const XmlCodecDataInfo * getCodecDataInfo() const override;

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void destroy() override;

	protected:
		XmlCodecOptions m_options;		

		LogServiceInterface * m_logService;		
        UnicodeServiceInterface * m_unicodeService;
	};
}