#	pragma once

namespace Menge
{
	class CodecEngine
		: public Holder<CodecEngine>
	{
	public:
		DecoderInterface * createDecoder( const String& _fileSystemName, const String& _filename, const String& _type );

		template<class T>
		T * createDecoderT( const String& _fileSystemName, const String& _filename, const String& _type )
		{
			return static_cast<T*>( createDecoder( _fileSystemName, _filename, _type ) );
		}

		template<class T>
		T * createDecoderT( const String& _filename, const String& _type, FileInput* _file )
		{
			return static_cast<T*>( createDecoder( _filename, _type, _file ) );
		}

		void releaseDecoder( Decoder * _decoder );

	public:
		DecoderInterface * createDecoder( const String& _filename, const String& _type, FileInputInterface * _file );

	protected:
		CodecSystemInterface * m_interface;
	};
}