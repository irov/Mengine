#	pragma once

#	include "Codec/PickEncoder.h"

namespace Menge
{
	class PickEncoderHIT
		: public PickEncoder
	{
	public:
		PickEncoderHIT();
		~PickEncoderHIT();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo ) override;
	};
}	// namespace Menge
