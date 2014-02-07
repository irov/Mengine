#	pragma once

#	include "Codec/ImageDecoder.h"

namespace Menge
{
	class ImageDecoderMemory
		: public ImageDecoder
	{
	public:
		ImageDecoderMemory();

	protected:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
	};    

	typedef stdex::intrusive_ptr<ImageDecoderMemory> ImageDecoderMemoryPtr;	
}
