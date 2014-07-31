#	pragma once

#	include "Codec/ImageDecoder.h"

namespace Menge
{
	class ImageDecoderDDS
		: public ImageDecoder
	{
	public:
		ImageDecoderDDS();
		~ImageDecoderDDS();

	public:
		bool _initialize() override;
		
	public:
		bool _prepareData() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
	};
}	// namespace Menge