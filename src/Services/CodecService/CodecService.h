#pragma once

#include "Interface/CodecServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class CodecService
        : public ServiceBase<CodecServiceInterface>
    {
        DECLARE_FACTORABLE( CodecService );

    public:
        CodecService();
        ~CodecService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        DecoderInterfacePointer createDecoder( const ConstString & _type, const DocumentInterfacePtr & _doc ) override;
        EncoderInterfacePointer createEncoder( const ConstString & _type, const DocumentInterfacePtr & _doc ) override;

    public:
        bool registerCodecExt( const ConstString & _ext, const ConstString & _codecType ) override;
        void removeCodecExt( const ConstString & _ext ) override;
        const ConstString & findCodecType( const FilePath & _filePath ) const override;

    protected:
        typedef Map<ConstString, ConstString> MapCodecExts;
        MapCodecExts m_codecExts;
    };
}
