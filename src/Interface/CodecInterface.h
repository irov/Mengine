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

#	define MENGE_DECLARE_ENCODER( _class_ )\
	Menge::String m_type;\
		public:\
		static Menge::EncoderInterface* _class_::creatorNew( OutStreamInterface*, const String& );\
		static Menge::EncoderInterface* _class_::creatorPlacementNew( void*, OutStreamInterface*, const String& );\
		private:

#	define MENGE_IMPLEMENT_ENCODER( _class_ )\
	Menge::EncoderInterface* _class_::creatorNew( OutStreamInterface* _stream, const String& _type ) { return new _class_( _stream, _type ); }\
	Menge::EncoderInterface* _class_::creatorPlacementNew( void* _mem, OutStreamInterface* _stream, const String& _type ) { return new (_mem) _class_( _stream, _type ); }\
	const Menge::String& _class_::getType() const { return m_type; }

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