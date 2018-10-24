#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"

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
        bool unregisterDecoder( const ConstString& _type ) override;

    public:
        bool registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _factory ) override;
        bool unregisterEncoder( const ConstString& _type ) override;

    public:
        DecoderInterfacePtr createDecoder( const ConstString& _type ) override;

    public:
        EncoderInterfacePtr createEncoder( const ConstString& _type ) override;

    public:
        bool registerCodecExt( const String & _ext, const ConstString & _codecType ) override;
        const ConstString & findCodecType( const FilePath & _path ) const override;

    protected:
        typedef Map<ConstString, DecoderFactoryInterfacePtr> MapDecoderSystem;
        MapDecoderSystem m_mapDecoderSystem;

        typedef Map<ConstString, EncoderFactoryInterfacePtr> MapEncoderSystem;
        MapEncoderSystem m_mapEncoderSystem;

        typedef Map<String, ConstString> MapCodecTypes;
        MapCodecTypes m_codecTypes;
    };
}
