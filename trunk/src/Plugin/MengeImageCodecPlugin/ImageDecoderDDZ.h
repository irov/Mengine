#	pragma once

#	include "Codec/ImageDecoder.h"

#   include "DDSHeader.h"

namespace Menge
{
	class ImageDecoderDDZ
		: public ImageDecoder
	{
	public:
		ImageDecoderDDZ();
		~ImageDecoderDDZ();

	public:
		bool _initialize() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
	
	private:       
        size_t m_dds_size;
        size_t m_compress_size;
        DDS_HEADER m_header;
	};
}	// namespace Menge