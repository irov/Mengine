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
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const InputStreamInterfacePtr & _stream, bool & _version ) override;

    protected:
        const InputStreamInterfacePtr & getStream() const override;

	public:
		bool setOptions( CodecOptions * _options ) override;

	public:
		const XmlCodecDataInfo * getCodecDataInfo() const override;

	public:		
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		XmlCodecOptions m_options;
	};
}