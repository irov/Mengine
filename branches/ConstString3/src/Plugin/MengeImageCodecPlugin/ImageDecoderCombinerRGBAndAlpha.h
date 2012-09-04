#	pragma once
#	include "ImageDecoder.h"


namespace Menge
{
	class LogServiceInterface;

	class ImageDecoderCombinerRGBAndAlpha
		: public ImageDecoderInterface
	{
	public:
		ImageDecoderCombinerRGBAndAlpha( ImageDecoderInterface * _decoderRGB, ImageDecoderInterface * _decoderAlpha, LogServiceInterface * _logService );
		~ImageDecoderCombinerRGBAndAlpha();
	
	public:
		bool initialize();

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize );
		const ImageCodecDataInfo * getCodecDataInfo() const;
		void setOptions( CodecOptions * _options );

	public:
		virtual void destroy();
	
	protected:
		//maybe must deleted
		void _invalidate();
		
		ImageDecoderInterface * m_decoderAlpha;
		ImageDecoderInterface * m_decoderRGB;
		ImageCodecDataInfo m_dataInfo;
		ImageCodecOptions m_options;

	private:
		LogServiceInterface * m_logService;
		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
	};
}	// namespace Menge

