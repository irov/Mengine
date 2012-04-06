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
		void registerDecoder( const ConstString& _type, DecoderSystemInterface * _interface ) override;
		void unregisterDecoder( const ConstString& _type ) override;

		void registerEncoder( const ConstString& _type, EncoderSystemInterface * _interface ) override;
		void unregisterEncoder( const ConstString& _type ) override;
		
		DecoderInterface * createDecoder( const ConstString& _type, InputStreamInterface * _stream ) override;
	
	public:
		bool registerCodecExt( const WString & _ext, const ConstString & _codecType ) override;
		const ConstString & findCodecType( const WString & _ext ) override;

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

		typedef std::map<WString, ConstString> TMapCodecTypes;
		TMapCodecTypes m_codecTypes;
	};
}
