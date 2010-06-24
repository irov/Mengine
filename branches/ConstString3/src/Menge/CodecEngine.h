#	pragma once

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
		: public CodecEngineInterface
		, public Holder<CodecEngine>
	{
	public:
		CodecEngine();
		~CodecEngine();

	public:
		void initialize();

	public:
		DecoderInterface * createDecoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type );
		DecoderInterface * createDecoder( const String& _filename, const ConstString& _type, FileInputInterface * _file );

		template<class T>
		T * createDecoderT( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
		{
			return dynamic_cast<T*>( createDecoder( _fileSystemName, _filename, _type ) );
		}

		template<class T>
		T * createDecoderT( const String& _filename, const ConstString& _type, FileInputInterface * _file )
		{
			return dynamic_cast<T*>( createDecoder( _filename, _type, _file ) );
		}

		void releaseDecoder( DecoderInterface * _decoder );

	public:
		EncoderInterface * createEncoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type );
		EncoderInterface * createEncoder( const String& _filename, const ConstString& _type, FileOutputInterface * _file );

		template<class T>
		T * createEncoderT( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
		{
			return dynamic_cast<T*>( createEncoder( _fileSystemName, _filename, _type ) );
		}

		template<class T>
		T * createEncoderT( const String& _filename, const ConstString& _type, FileOutputInterface * _file )
		{
			return dynamic_cast<T*>( createEncoder( _filename, _type, _file ) );
		}

		void releaseEncoder( EncoderInterface * _decoder );

	protected:
		ConstString getType_( const String& _filename, const ConstString& _type ) const;
		
	protected:
		CodecSystemInterface * m_interface;

	protected:
		FactoryManager m_decoderFactory;
		FactoryManager m_encoderFactory;
	};
}
