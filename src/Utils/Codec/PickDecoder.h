#	pragma once

#	include "Interface/PickCodecInterface.h"

namespace Menge
{
	class PickDecoder
		: public PickDecoderInterface
	{
	public:
		PickDecoder();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const InputStreamInterfacePtr & _stream ) override;

    protected:
        virtual bool _initialize();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:		
        const InputStreamInterfacePtr & getStream() const override;

	public:
		void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
		const PickCodecDataInfo * getCodecDataInfo() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		PickCodecOptions m_options;
		PickCodecDataInfo m_dataInfo;
	};
}
