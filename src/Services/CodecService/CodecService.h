#pragma once

#include "Interface/CodecServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable.h"

#include "Config/Map.h"

namespace Mengine
{
    class CodecService
        : public ServiceBase<CodecServiceInterface>
    {
    public:
        CodecService();
        ~CodecService() override;

    public:
        bool registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _factory ) override;
        DecoderFactoryInterfacePtr unregisterDecoder( const ConstString& _type ) override;

    public:
        bool registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _factory ) override;
        EncoderFactoryInterfacePtr unregisterEncoder( const ConstString& _type ) override;

    public:
        DecoderInterfacePtr createDecoder( const ConstString& _type ) override;

    public:
        EncoderInterfacePtr createEncoder( const ConstString& _type ) override;

    public:
        bool registerCodecExt( const String & _ext, const ConstString & _codecType ) override;
        const ConstString & findCodecType( const FilePath & _path ) const override;

    protected:
        typedef Hashtable<ConstString, DecoderFactoryInterfacePtr> MapFactorDecoders;
        MapFactorDecoders m_factorDecoders;

        typedef Hashtable<ConstString, EncoderFactoryInterfacePtr> MapFactoryEncoders;
        MapFactoryEncoders m_factorEncoders;

        typedef Map<String, ConstString> MapCodecTypes;
        MapCodecTypes m_codecTypes;
    };
}
