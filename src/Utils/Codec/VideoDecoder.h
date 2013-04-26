#	pragma once

#	include "Interface/VideoCodecInterface.h"

namespace Menge
{
	class VideoDecoder
        : public VideoDecoderInterface
	{
	public:
		VideoDecoder();

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
		const VideoCodecDataInfo * getCodecDataInfo() const override;
        
    protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		VideoCodecOptions m_options;
		VideoCodecDataInfo m_dataInfo;
	};
}
