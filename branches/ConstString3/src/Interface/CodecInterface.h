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

	class CodecEngineInterface
	{
	public:
		virtual void registerDecoder( const ConstString& _type, DecoderInterface * _interface ) = 0;
		virtual void registerEncoder( const ConstString& _type, DecoderInterface * _interface ) = 0;
	};


}	// namespace Menge

bool initInterfaceSystem( Menge::CodecSystemInterface** );
void releaseInterfaceSystem( Menge::CodecSystemInterface* );
