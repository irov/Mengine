#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	struct CodecDataInfo 
	{
		std::size_t size;
	};

	struct CodecOptionsInfo
	{
	};

	class DecoderInterface
	{
	public:
		virtual const CodecDataInfo* getCodecDataInfo() const = 0;
		virtual FileInputInterface * getStream() = 0;

		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;

		virtual void release() = 0;
	};

	class DecoderSystemInterface
	{
	public:
		virtual DecoderInterface * createDecoder( FileInputInterface * _stream, CodecOptionsInfo * _info ) = 0;
	};

	class EncoderInterface
	{
	public:
		virtual FileOutputInterface * getStream() = 0;
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;

		virtual void release() = 0;
	};

	class EncoderSystemInterface
	{
	public:
		virtual EncoderInterface * createEncoder( FileOutputInterface * _stream, CodecOptionsInfo * _info ) = 0;
	};

	class CodecServiceInterface
		: public ServiceInterface
	{
	public:
		virtual void registerDecoder( const String& _type, DecoderSystemInterface * _interface ) = 0;
		virtual void registerEncoder( const String& _type, EncoderSystemInterface * _interface ) = 0;
	};


}	// namespace Menge
