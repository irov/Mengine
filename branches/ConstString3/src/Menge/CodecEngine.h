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

	class FileInputStreamInterface;
	class FileOutputStreamInterface;

	class CodecEngine
		: public Holder<CodecEngine>
		, public CodecServiceInterface
	{
	public:
		CodecEngine();
		~CodecEngine();

	public:
		void registerDecoder( const String& _type, DecoderSystemInterface * _interface ) override;
		void unregisterDecoder( const String& _type ) override;

		void registerEncoder( const String& _type, EncoderSystemInterface * _interface ) override;
		void unregisterEncoder( const String& _type ) override;
		
		DecoderInterface * createDecoder( const ConstString& _type, InputStreamInterface * _stream ) override;
		DecoderInterface * createDecoder( const char * _type, InputStreamInterface * _stream ) override;
	
	protected:
		bool supportA8() override;
	public:
		EncoderInterface * createEncoder( const ConstString& _type, OutputStreamInterface * stream );

		template<class T>
		T * createEncoderT( const ConstString& _type, OutputStreamInterface * _stream )
		{
			return dynamic_cast<T*>( createEncoder( _type, _stream ) );
		}

	protected:
		typedef std::map<ConstString, DecoderSystemInterface *> TMapDecoderSystem;
		TMapDecoderSystem m_mapDecoderSystem;

		typedef std::map<ConstString, EncoderSystemInterface *> TMapEncoderSystem;
		TMapEncoderSystem m_mapEncoderSystem;
	};
}
