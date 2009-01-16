#	pragma once

#	include "Config/Typedef.h"

#	include <map>

#	define MENGE_REGISTER_DECODER( _typename_, _class_, _type_ )\
	Menge::DecoderManager::registerDecoder( String(_type_) + String(_typename_), _class_::creatorNew, _class_::creatorPlacementNew );

#	define MENGE_REGISTER_ENCODER( _typename_, _class_, _type_ )\
	Menge::EncoderManager::registerEncoder( String(_type_) + String(_typename_), _class_::creatorNew, _class_::creatorPlacementNew );

namespace Menge 
{
	class DecoderInterface;
	class EncoderInterface;
	class DataStreamInterface;
	class OutStreamInterface;

	typedef DecoderInterface* (*TDecoderCreatorNew)( DataStreamInterface* _stream, const String& _type );
	typedef DecoderInterface* (*TDecoderCreatorPlacementNew)( void* _mem, DataStreamInterface* _stream, const String& _type );

	typedef EncoderInterface* (*TEncoderCreatorNew)( OutStreamInterface* _stream, const String& _type );
	typedef EncoderInterface* (*TEncoderCreatorPlacementNew)( void* _mem, OutStreamInterface* _stream, const String& _type );

	class DecoderManager
	{
	public:
		static void cleanup();
		static void registerDecoder( const String& _type
			, TDecoderCreatorNew _creatorNew
			, TDecoderCreatorPlacementNew _creatorPlacementNew );

		static void unregisterDecoder( const String& _type );

		static DecoderInterface * createDecoder( const String& _filename, const String& _type );

		template<class T>
		static T* createDecoderT( const String& _filename, const String& _type )
		{
			return static_cast<T*>( createDecoder( _filename, _type ) );
		}

		static void releaseDecoder( DecoderInterface* _decoder );

	protected:
		typedef std::map< String, std::pair<TDecoderCreatorNew, TDecoderCreatorPlacementNew> > TDecoderFactory;
		static TDecoderFactory ms_decoderFactory;

		typedef std::vector<DecoderInterface *> TDecoderBuff;
		typedef std::map<String, TDecoderBuff> TDecoderPool;
		static TDecoderPool ms_decoderPool;
	};

	// Encoder
	class EncoderManager
	{
	public:
		static void cleanup();
		static void registerEncoder( const String& _type
			, TEncoderCreatorNew _creatorNew
			, TEncoderCreatorPlacementNew _creatorPlacementNew );

		static void unregisterEncoder( const String& _type );

		static EncoderInterface * createEncoder( const String& _filename, const String& _type );

		template<class T>
		static T* createEncoderT( const String& _filename, const String& _type )
		{
			return static_cast<T*>( createEncoder( _filename, _type ) );
		}

		static void releaseEncoder( EncoderInterface* _encoder );

	protected:
		typedef std::map< String, std::pair<TEncoderCreatorNew, TEncoderCreatorPlacementNew> > TEncoderFactory;
		static TEncoderFactory ms_encoderFactory;

		typedef std::vector<EncoderInterface *> TEncoderBuff;
		typedef std::map<String, TEncoderBuff> TEncoderPool;
		static TEncoderPool ms_encoderPool;
	};
} // namespace Menge
