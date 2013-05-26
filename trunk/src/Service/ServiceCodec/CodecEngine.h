#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/ConstString.h"
#   include "Core/BinaryVector.h"

namespace Menge
{
	class CodecFactoryInterface;
    class RenderServiceInterface;

	class DecoderInterface;
	class EncoderInterface;

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
		DecoderInterfacePtr createDecoder( const ConstString& _type, const InputStreamInterfacePtr & _stream ) override;

	public:
		EncoderInterfacePtr createEncoder( const ConstString& _type, const OutputStreamInterfacePtr & stream ) override;

    public:
        bool registerCodecExt( const String & _ext, const ConstString & _codecType ) override;
        const ConstString & findCodecType( const FilePath & _path ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef BinaryVector<ConstString, DecoderFactoryInterface *> TMapDecoderSystem;
		TMapDecoderSystem m_mapDecoderSystem;

		typedef BinaryVector<ConstString, EncoderFactoryInterface *> TMapEncoderSystem;
		TMapEncoderSystem m_mapEncoderSystem;

		typedef BinaryVector<String, ConstString> TMapCodecTypes;
		TMapCodecTypes m_codecTypes;
	};
}
