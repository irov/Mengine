#	pragma once

#	include "Codec/ImageDecoder.h"

namespace Menge
{
	class ImageDecoderCRN
		: public ImageDecoder
	{
	public:
		ImageDecoderCRN();
		~ImageDecoderCRN();

	public:
		bool _prepareData() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
	
	private:       
		uint32_t m_data_size;
	};
}	// namespace Menge