#	pragma once

#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	class ImageDecoder
		: public ImageDecoderInterface
	{
	public:
		ImageDecoder();

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
		const ImageCodecDataInfo * getCodecDataInfo() const override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

        ImageCodecOptions m_options;
		ImageCodecDataInfo m_dataInfo;
	};    
}
