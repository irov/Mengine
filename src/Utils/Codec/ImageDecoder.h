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
		const ImageCodecDataInfo * getCodecDataInfo() const override;

    protected:
        void sweezleAlpha1( size_t _width, size_t _height, void * _buffer, size_t _pitch );
        void sweezleAlpha3( size_t _width, size_t _height, void * _buffer, size_t _pitch );

    protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

        ImageCodecOptions m_options;
		ImageCodecDataInfo m_dataInfo;
	};    
}
