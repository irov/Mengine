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
        bool initialize( const InputStreamInterfacePtr & _stream, bool & _version ) override;

    protected:
        virtual bool _initialize( bool & _version );

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        const InputStreamInterfacePtr & getStream() const override;
		const VideoCodecDataInfo * getCodecDataInfo() const override;
        
    protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		VideoCodecOptions m_options;
		VideoCodecDataInfo m_dataInfo;
	};
}
