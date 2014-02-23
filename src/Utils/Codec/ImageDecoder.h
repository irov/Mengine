#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "Codec/Decoder.h"

namespace Menge
{
	class ImageDecoder
		: public Decoder<ImageDecoderInterface>
	{
	public:
		ImageDecoder();
		
	public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

	public:
		void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
		const ImageCodecDataInfo * getCodecDataInfo() const override;

    protected:
        void sweezleAlpha1( size_t _width, size_t _height, void * _buffer, size_t _pitch );
        void sweezleAlpha3( size_t _width, size_t _height, void * _buffer, size_t _pitch );

    protected:
        ImageCodecOptions m_options;
		ImageCodecDataInfo m_dataInfo;
	};    
}
