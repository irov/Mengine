#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class CodecSystemInterface;

	class DecoderInterface;
	class EncoderInterface;

	class FileInputInterface;
	class FileOutputInterface;

	class CodecEngine
		: public Holder<CodecEngine>
		, public CodecServiceInterface
	{
	public:
		CodecEngine();
		~CodecEngine();

	public:
		void registerDecoder( const String& _type, DecoderSystemInterface * _interface ) override;
		void registerEncoder( const String& _type, EncoderSystemInterface * _interface ) override;

	public:
		DecoderInterface * createDecoder( const ConstString& _type, InputStreamInterface * _stream );

		template<class T>
		T * createDecoderT( const ConstString& _type, InputStreamInterface * _stream )
		{
			return dynamic_cast<T*>( createDecoder( _type, _stream ) );
		}

		void releaseDecoder( DecoderInterface * _decoder );

	public:
		EncoderInterface * createEncoder( const ConstString& _type, OutputStreamInterface * stream );

		template<class T>
		T * createEncoderT( const ConstString& _type, OutputStreamInterface * _stream )
		{
			return dynamic_cast<T*>( createEncoder( _type, _stream ) );
		}

		void releaseEncoder( EncoderInterface * _decoder );

	protected:
		typedef std::map<ConstString, DecoderSystemInterface *> TMapDecoderSystem;
		TMapDecoderSystem m_mapDecoderSystem;

		typedef std::map<ConstString, EncoderSystemInterface *> TMapEncoderSystem;
		TMapEncoderSystem m_mapEncoderSystem;
	};
}
