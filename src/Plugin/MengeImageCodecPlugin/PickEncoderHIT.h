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
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;
	};
}	// namespace Menge
