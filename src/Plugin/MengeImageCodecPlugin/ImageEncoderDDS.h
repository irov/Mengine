#	pragma once

#	include "Codec/ImageEncoder.h"

namespace Menge
{
	class ImageEncoderDDS
		: public ImageEncoder
	{
	public:
		ImageEncoderDDS();
		~ImageEncoderDDS();

	public:
		bool _initialize() override;
		
	public:
		size_t encode( const void * _buffer, size_t _size, const CodecDataInfo* _dataInfo );
	};
}	// namespace Menge