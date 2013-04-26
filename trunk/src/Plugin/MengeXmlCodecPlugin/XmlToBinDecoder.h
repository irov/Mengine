#	pragma once

#	include "Interface/XmlCodecInterface.h"
#   include "Interface/InputSystemInterface.h"

namespace Menge
{
	class XmlToBinDecoder
        : public XmlDecoderInterface
	{
	public:
		XmlToBinDecoder();
	
    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream ) override;

    public:
        ServiceProviderInterface * getServiceProvider() const;

    protected:
        InputStreamInterfacePtr getStream() const override;

	public:
		bool setOptions( CodecOptions * _options ) override;

	public:
		const XmlCodecDataInfo * getCodecDataInfo() const override;

	public:		
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		XmlCodecOptions m_options;
	};
}