#	pragma once

#	ifdef MENGINE_SUPPORT_DECODER_CRN

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

#	endif