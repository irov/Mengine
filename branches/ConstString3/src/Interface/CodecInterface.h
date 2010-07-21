#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/StreamInterface.h"

namespace Menge
{
	struct CodecDataInfo 
	{
		std::size_t size;
	};

	struct CodecOptions
	{
		CodecOptions()
			: flags(0)
		{}

		std::size_t flags;
	};

	class DecoderInterface
	{
	public:
		virtual void setOptions( CodecOptions * _info ) = 0;

	public:
		virtual const CodecDataInfo* getCodecDataInfo() const = 0;
		virtual InputStreamInterface * getStream() const = 0;

	public:
		virtual bool initialize() = 0;
		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;

		virtual void release() = 0;
	};

	class DecoderSystemInterface
	{
	public:
		virtual DecoderInterface * createDecoder( InputStreamInterface * _stream ) = 0;
	};

	class EncoderInterface
	{
	public:
		virtual void setOptions( CodecOptions * _info ) = 0;
		virtual OutputStreamInterface * getStream() const = 0;

	public:
		virtual bool initialize() = 0;
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;

		virtual void release() = 0;
	};

	class EncoderSystemInterface
	{
	public:
		virtual EncoderInterface * createEncoder( OutputStreamInterface * _stream ) = 0;
	};

	class CodecServiceInterface
		: public ServiceInterface
	{
	public:
		virtual void registerDecoder( const String& _type, DecoderSystemInterface * _interface ) = 0;
		virtual void registerEncoder( const String& _type, EncoderSystemInterface * _interface ) = 0;
	};


}	// namespace Menge
