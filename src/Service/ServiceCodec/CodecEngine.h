#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class CodecFactoryInterface;
    class RenderServiceInterface;

	class DecoderInterface;
	class EncoderInterface;

	class FileInputStreamInterface;
	class FileOutputStreamInterface;

	class CodecEngine
		: public CodecServiceInterface
	{
	public:
		CodecEngine();
		~CodecEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		void registerDecoder( const ConstString& _type, DecoderFactoryInterface * _interface ) override;
		void unregisterDecoder( const ConstString& _type ) override;

		void registerEncoder( const ConstString& _type, EncoderFactoryInterface * _interface ) override;
		void unregisterEncoder( const ConstString& _type ) override;
	
    public:
		DecoderInterface * createDecoder( const ConstString& _type, InputStreamInterface * _stream ) override;

	public:
		EncoderInterface * createEncoder( const ConstString& _type, OutputStreamInterface * stream ) override;

    public:
        bool registerCodecExt( const String & _ext, const ConstString & _codecType ) override;
        const ConstString & findCodecType( const FilePath & _path ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, DecoderFactoryInterface *> TMapDecoderSystem;
		TMapDecoderSystem m_mapDecoderSystem;

		typedef std::map<ConstString, EncoderFactoryInterface *> TMapEncoderSystem;
		TMapEncoderSystem m_mapEncoderSystem;

		typedef std::map<String, ConstString> TMapCodecTypes;
		TMapCodecTypes m_codecTypes;
	};
}
