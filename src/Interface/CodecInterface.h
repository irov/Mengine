#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class DataStreamInterface;
	class OutStreamInterface;

	struct CodecDataInfo 
	{
		std::size_t size;
	};

	class DecoderInterface
	{
	public:
		virtual void destructor() = 0;
		virtual void release() = 0;
		virtual const String& getType() const = 0;
	
		virtual DataStreamInterface* getStream() = 0;
		virtual const CodecDataInfo* getCodecDataInfo() const = 0;
		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;

	};

	class EncoderInterface
	{
	public:
		virtual void destructor() = 0;
		virtual void release() = 0;
		virtual const String& getType() const = 0;

		virtual OutStreamInterface* getStream() = 0;
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;
	};

}	// namespace Menge
