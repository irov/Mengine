#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	//class DataStreamInterface;
	//class OutStreamInterface;

	struct CodecDataInfo 
	{
		std::size_t size;
	};

	class DecoderInterface
	{
	public:
		virtual const CodecDataInfo* getCodecDataInfo() const = 0;
		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;
	};

	class EncoderInterface
	{
	public:
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;
	};

}	// namespace Menge
