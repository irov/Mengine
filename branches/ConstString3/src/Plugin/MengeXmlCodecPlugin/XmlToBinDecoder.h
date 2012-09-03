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
		Xml2BinDecoder( InputStreamInterface * _stream, LogServiceInterface * _logService, UnicodeServiceInterface * _unicodeService );
	
	public:
		void setOptions( CodecOptions * _options ) override;

	public:
		const XmlCodecDataInfo * getCodecDataInfo() const override;
		InputStreamInterface * getStream() const override;

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void destroy() override;

	protected:
		XmlCodecOptions m_options;
		InputStreamInterface * m_stream;

		LogServiceInterface * m_logService;		
        UnicodeServiceInterface * m_unicodeService;
	};
}

extern "C" bool writeBinary( const wchar_t * _protocol, const wchar_t * _source, const wchar_t * _bin, char * _error );