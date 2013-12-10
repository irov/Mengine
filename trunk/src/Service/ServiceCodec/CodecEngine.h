#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

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
		void registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _factory ) override;
		void unregisterDecoder( const ConstString& _type ) override;

		void registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _factory ) override;
		void unregisterEncoder( const ConstString& _type ) override;
	
    public:
		DecoderInterfacePtr createDecoder( const ConstString& _type ) override;

	public:
		EncoderInterfacePtr createEncoder( const ConstString& _type ) override;

    public:
        bool registerCodecExt( const String & _ext, const ConstString & _codecType ) override;
        const ConstString & findCodecType( const FilePath & _path ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef stdex::binary_vector<ConstString, DecoderFactoryInterfacePtr> TMapDecoderSystem;
		TMapDecoderSystem m_mapDecoderSystem;

		typedef stdex::binary_vector<ConstString, EncoderFactoryInterfacePtr> TMapEncoderSystem;
		TMapEncoderSystem m_mapEncoderSystem;

		typedef stdex::binary_vector<String, ConstString> TMapCodecTypes;
		TMapCodecTypes m_codecTypes;
	};
}
