#pragma once

#include "Codec/ImageDecoder.h"

namespace Mengine
{
	class ImageDecoderMemory
		: public ImageDecoder
	{
	public:
		ImageDecoderMemory();

	protected:
		size_t _decode( void * _buffer, size_t _bufferSize ) override;

	protected:
		size_t decodeData_( void * _buffer, size_t _bufferSize ) const;
	};    

	typedef IntrusivePtr<ImageDecoderMemory> ImageDecoderMemoryPtr;
}
