#	pragma once

#	include "Interface/XmlCodecInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class InputStreamInterface;

	class Xml2BinDecoder
		: public XmlDecoderInterface
	{
	public:
		Xml2BinDecoder( InputStreamInterface * _stream, LogSystemInterface * _logger );
	
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
		LogSystemInterface * m_logSystem;
	};
}