#	pragma once

#	include "Interface/CodecInterface.h"

#   include "Core/ServiceBase.h"

#	include "Core/ConstString.h"

#   include "stdex/stl_map.h"

namespace Menge
{
	class CodecFactoryInterface;
    class RenderServiceInterface;

	class DecoderInterface;
	class EncoderInterface;

	class CodecEngine
		: public ServiceBase<CodecServiceInterface>
	{
	public:
		CodecEngine();
		~CodecEngine();

	public:
		void registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _factory ) override;
		void unregisterDecoder( const ConstString& _type ) override;

	public:
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
		typedef stdex::map<ConstString, DecoderFactoryInterfacePtr> TMapDecoderSystem;
		TMapDecoderSystem m_mapDecoderSystem;

		typedef stdex::map<ConstString, EncoderFactoryInterfacePtr> TMapEncoderSystem;
		TMapEncoderSystem m_mapEncoderSystem;

		typedef stdex::map<String, ConstString> TMapCodecTypes;
		TMapCodecTypes m_codecTypes;
	};
}
