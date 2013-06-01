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
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
	
	private:
        TBlobject m_buffer;
        size_t m_dds_size;
        DDS_HEADER m_header;
	};
}	// namespace Menge