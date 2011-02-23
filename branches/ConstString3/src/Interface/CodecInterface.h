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

	class CodecServiceInterface;

	class CodecSystemInterface
	{
	public:
		virtual void setService( CodecServiceInterface * _service ) = 0;
	};

	class DecoderInterface
	{
	public:
		virtual void setOptions( CodecOptions * _options ) = 0;

	public:
		virtual const CodecDataInfo* getCodecDataInfo() const = 0;
		virtual InputStreamInterface * getStream() const = 0;

	public:
		virtual bool initialize() = 0;
		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;

		virtual void destroy() = 0;
	};

	class DecoderSystemInterface
		: public CodecSystemInterface
	{
	public:
		virtual DecoderInterface * createDecoder( InputStreamInterface * _stream ) = 0;
	};

	class EncoderInterface
	{
	public:
		virtual void setOptions( CodecOptions * _options ) = 0;
		virtual OutputStreamInterface * getStream() const = 0;

	public:
		virtual bool initialize() = 0;
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;

		virtual void release() = 0;
	};

	class EncoderSystemInterface
		: public CodecSystemInterface
	{
	public:
		virtual EncoderInterface * createEncoder( OutputStreamInterface * _stream ) = 0;
	};

	enum ECodecTargetFormat
	{
		ECTF_Texture
	};

	class CodecServiceInterface
		: public ServiceInterface
	{
	public:
		virtual void registerDecoder( const String& _type, DecoderSystemInterface * _interface ) = 0;
		virtual void unregisterDecoder( const String& _type ) = 0;

		virtual void registerEncoder( const String& _type, EncoderSystemInterface * _interface ) = 0;
		virtual void unregisterEncoder( const String& _type ) = 0;

	public: //support something shits
		virtual bool supportA8() = 0;
	};


}	// namespace Menge
