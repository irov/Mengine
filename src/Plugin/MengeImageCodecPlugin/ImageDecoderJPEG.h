#	pragma once

#	include "Codec/ImageDecoder.h"

struct jpeg_decompress_struct;

namespace Menge
{
	struct tagErrorManager;

	class LogServiceInterface;

	class ImageDecoderJPEG
		: public ImageDecoder
	{
	public:
		ImageDecoderJPEG();
		~ImageDecoderJPEG();

    protected:
		bool _initialize() override;

    protected:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
        
	private:
		int getQuality( jpeg_decompress_struct* _jpegObject );

	private:
		jpeg_decompress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;
	};
}	// namespace Menge
