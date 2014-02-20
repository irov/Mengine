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
		void setCodecDataInfo( const CodecDataInfo * ) override;
		const VideoCodecDataInfo * getCodecDataInfo() const override;
        
    protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		VideoCodecOptions m_options;
		VideoCodecDataInfo m_dataInfo;
	};
}
