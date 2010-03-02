#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/FileSystemInterface.h"

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
		virtual FileInputInterface * getStream() = 0;

		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;
	};

	class EncoderInterface
	{
	public:
		virtual FileOutputInterface * getStream() = 0;
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;
	};

	class CodecSystemInterface
	{
	public:
		virtual DecoderInterface * createDecoder( const String& _filename, const String& _type, FileInputInterface * _file ) = 0;
		virtual void releaseDecoder( DecoderInterface * _decoder ) = 0;

		virtual EncoderInterface * createEncoder( const String& _filename, const String& _type, FileOutputInterface * _file ) = 0;
		virtual void releaseEncoder( EncoderInterface * _encoder ) = 0;
	};
}	// namespace Menge
