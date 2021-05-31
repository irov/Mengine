#pragma once

#include "Interface/DecoderInterface.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct XmlCodecDataInfo
        : public CodecDataInfo
    {
    };
    //////////////////////////////////////////////////////////////////////////
    struct XmlDecoderData
        : public DecoderData
    {
        FilePath pathProtocol;
        FilePath pathXml;
        FilePath pathBin;

        uint32_t useProtocolVersion;
        uint32_t useProtocolCrc32;
    };
    //////////////////////////////////////////////////////////////////////////
    class XmlDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const XmlCodecDataInfo * getCodecDataInfo() const override = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<XmlDecoderInterface> XmlDecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
