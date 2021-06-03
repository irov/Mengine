#pragma once

#include "Interface/DecoderInterface.h"
#include "Interface/EncoderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct PickCodecDataInfo
        : public CodecDataInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;

        uint32_t mipmaplevel = 0;
        uint32_t mipmapsize = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct PickDecoderData
        : public DecoderData
    {
    };
    //////////////////////////////////////////////////////////////////////////
    struct PickEncoderData
        : public EncoderData
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class PickDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const PickCodecDataInfo * getCodecDataInfo() const override = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PickDecoderInterface> PickDecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PickEncoderInterface
        : public EncoderInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PickEncoderInterface> PickEncoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
