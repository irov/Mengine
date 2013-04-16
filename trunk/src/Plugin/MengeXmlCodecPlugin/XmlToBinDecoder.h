#	pragma once

#	include "Interface/XmlCodecInterface.h"
#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

namespace Menge
{
	class InputStreamInterface;

	class XmlToBinDecoder
        : public XmlDecoderInterface
	{
	public:
		XmlToBinDecoder();
	
    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream ) override;

    public:
        ServiceProviderInterface * getServiceProvider() const;

    protected:
        InputStreamInterface * getStream() const override;

	public:
		bool setOptions( CodecOptions * _options ) override;

	public:
		const XmlCodecDataInfo * getCodecDataInfo() const override;

	public:		
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterface * m_stream;

		XmlCodecOptions m_options;
	};
}