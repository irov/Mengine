#	pragma once

#	include "Codec/ImageDecoder.h"

#	include "webp/decode.h"
#	include "webp/mux.h"

namespace Menge
{
	class ImageDecoderWEBP
		: public ImageDecoder
	{
	public:
		ImageDecoderWEBP();
		~ImageDecoderWEBP();

	public:
		bool _initialize( bool & _version ) override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	protected:
        WebPBitstreamFeatures m_features;

	private:
		void cleanup_();		
	};
}	// namespace Menge
