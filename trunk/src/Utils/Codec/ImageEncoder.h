#	pragma once

#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	class ImageEncoder
		: public ImageEncoderInterface
	{
	public:
		ImageEncoder();

    public:
        ServiceProviderInterface * getServiceProvider() const;

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, OutputStreamInterface * _stream ) override;

    protected:
        virtual bool _initialize();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        OutputStreamInterface * getStream() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        OutputStreamInterface * m_stream;

		ImageCodecOptions m_options;
	};
}
