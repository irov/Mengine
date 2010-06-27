#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Config/Typedef.h"
#	include "Utils/Core/Holder.h"

#	include "ConstManager.h"

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
		DecoderInterface * createDecoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type );
		DecoderInterface * createDecoder( const String& _filename, const ConstString& _type, FileInputInterface * _stream );

		template<class T>
		T * createDecoderT( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
		{
			return dynamic_cast<T*>( createDecoder( _fileSystemName, _filename, _type ) );
		}

		template<class T>
		T * createDecoderT( const String& _filename, const ConstString& _type, FileInputInterface * _stream )
		{
			return dynamic_cast<T*>( createDecoder( _filename, _type, _stream ) );
		}

		void releaseDecoder( DecoderInterface * _decoder );

	public:
		EncoderInterface * createEncoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type );
		EncoderInterface * createEncoder( const String& _filename, const ConstString& _type, FileOutputInterface * stream );

		template<class T>
		T * createEncoderT( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
		{
			return dynamic_cast<T*>( createEncoder( _fileSystemName, _filename, _type ) );
		}

		template<class T>
		T * createEncoderT( const String& _filename, const ConstString& _type, FileOutputInterface * _stream )
		{
			return dynamic_cast<T*>( createEncoder( _filename, _type, _stream ) );
		}

		void releaseEncoder( EncoderInterface * _decoder );

	protected:
		typedef std::map<ConstString, DecoderSystemInterface *> TMapDecoderSystem;
		TMapDecoderSystem m_mapDecoderSystem;

		typedef std::map<ConstString, EncoderSystemInterface *> TMapEncoderSystem;
		TMapEncoderSystem m_mapEncoderSystem;
	};
}
