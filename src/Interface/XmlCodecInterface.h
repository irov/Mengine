#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    struct XmlCodecDataInfo
        : public CodecDataInfo
    {
    };

    struct XmlCodecOptions
        : public CodecOptions
    {
        FilePath pathProtocol;
        FilePath pathXml;
        FilePath pathBin;
        //int version;
    };

    class XmlDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const XmlCodecDataInfo * getCodecDataInfo() const override = 0;
    };

    typedef IntrusivePtr<XmlDecoderInterface> XmlDecoderInterfacePtr;
}
