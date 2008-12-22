#	pragma once

#	include "Config/Typedef.h"

#	define MENGE_DECLARE_DECODER( _class_ )\
		Menge::String m_type;\
		public:\
		static Menge::DecoderInterface* _class_::creatorNew( DataStreamInterface*, const String& );\
		static Menge::DecoderInterface* _class_::creatorPlacementNew( void*, DataStreamInterface*, const String& );\
		private:

#	define MENGE_IMPLEMENT_DECODER( _class_ )\
	Menge::DecoderInterface* _class_::creatorNew( DataStreamInterface* _stream, const String& _type ) { return new _class_( _stream, _type ); }\
	Menge::DecoderInterface* _class_::creatorPlacementNew( void* _mem, DataStreamInterface* _stream, const String& _type ) { return new (_mem) _class_( _stream, _type ); }\
	const Menge::String& _class_::getType() const { return m_type; }

namespace Menge
{
	class DataStreamInterface;
	//class OutStreamInterface;

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

	/*class SoundDecoderInterface
		: public DecoderInterface
	{
	public:

	};*/

	/*class CodecInterface
	{
	public:

		virtual void destructor() = 0;
		virtual void release() = 0;
		virtual const String& getType() const = 0;

		virtual void setDecodeOptions( unsigned int _options ) = 0;
		virtual const CodecData* startDecode( DataStreamInterface* _stream ) = 0;
		virtual void decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;
		virtual bool eof() const = 0;
		virtual void finishDecode() = 0;

		virtual void startEncode( OutStreamInterface* _stream ) = 0;
		virtual void encode( unsigned char* _buffer, CodecData* _codecData ) = 0;
		virtual void finishEncode() = 0;

	};

	class StreamCodecInterface
		: public CodecInterface
	{
	public:

		virtual int sync( float _timing ) = 0;
		virtual void seek( float _timing ) = 0;
	};

	class SoundCodecData
		: public CodecData
	{
	};*/
}	// namespace Menge