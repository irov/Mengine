#	pragma once
#	include "ImageDecoder.h"


namespace Menge
{
	class ImageDecoderCombinerRGBAndAlpha
		: public ImageDecoderInterface
	{
	public:
		ImageDecoderCombinerRGBAndAlpha( ImageDecoderInterface * _decoderRGB, ImageDecoderInterface * _decoderAlpha, LogSystemInterface * _logSystem );
		~ImageDecoderCombinerRGBAndAlpha();
	
	public:
		bool initialize();
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize );
		const ImageCodecDataInfo * getCodecDataInfo() const;
		void setOptions( CodecOptions * _options );
		virtual InputStreamInterface * getStream() const;
		virtual void destroy();
	
	protected:
		//maybe must deleted
		void _invalidate();
		
		ImageDecoderInterface * m_decoderAlpha;
		ImageDecoderInterface * m_decoderRGB;
		ImageCodecDataInfo m_dataInfo;
		ImageCodecOptions m_options;

	private:
		LogSystemInterface * m_logSystem;
		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
	};
}	// namespace Menge

