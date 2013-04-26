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
        ServiceProviderInterface * getServiceProvider() const;

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream ) override;

    protected:
        virtual bool _initialize();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        InputStreamInterfacePtr getStream() const override;
		const PickCodecDataInfo * getCodecDataInfo() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		PickCodecOptions m_options;
		PickCodecDataInfo m_dataInfo;
	};
}
