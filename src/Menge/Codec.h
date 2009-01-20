#	pragma once

#	include "Config/Typedef.h"

#	include "Holder.h"
#	include "Factory.h"

#	include <map>

#	define MENGE_REGISTER_DECODER( _typename_, C, _type_ )\
	Holder<DecoderManager>::hostage()\
		->registerDecoder( String(_type_) + String(_typename_), C::genObject );

#	define MENGE_REGISTER_ENCODER( _typename_, C, _type_ )\
	Holder<EncoderManager>::hostage()\
		->registerEncoder( String(_type_) + String(_typename_), C::genObject );

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
		void registerDecoder( const String& _type, Factory::TGenFunc _creator );
		void unregisterDecoder( const String& _type );

		DecoderInterface * createDecoder( const String& _filename, const String& _type );

		template<class T>
		T * createDecoderT( const String& _filename, const String& _type )
		{
			return static_cast<T*>( createDecoder( _filename, _type ) );
		}

		void releaseDecoder( DecoderInterface* _decoder );

	protected:
		Factory m_factory;

		typedef std::vector<DecoderInterface *> TDecoderBuff;
		typedef std::map<String, TDecoderBuff> TDecoderPool;
		TDecoderPool m_decoderPool;
	};

	// Encoder
	class EncoderManager
	{
	public:
		void registerEncoder( const String& _type, Factory::TGenFunc _creator );
		void unregisterEncoder( const String& _type );

		EncoderInterface * createEncoder( const String& _filename, const String& _type );

		template<class T>
		T* createEncoderT( const String& _filename, const String& _type )
		{
			return static_cast<T*>( createEncoder( _filename, _type ) );
		}

		void releaseEncoder( EncoderInterface* _encoder );

	protected:
		Factory m_factory;

		typedef std::vector<EncoderInterface *> TEncoderBuff;
		typedef std::map<String, TEncoderBuff> TEncoderPool;
		TEncoderPool m_encoderPool;
	};
} // namespace Menge
