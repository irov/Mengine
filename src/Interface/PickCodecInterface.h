#pragma once

#include "Interface/DecoderInterface.h"
#include "Interface/EncoderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct PickCodecDataInfo
        : public CodecDataInfo
    {
        PickCodecDataInfo()
            : width( 0 )
            , height( 0 )
            , mipmaplevel( 0 )
            , mipmapsize( 0 )
        {
        }

        uint32_t width;
        uint32_t height;

        uint32_t mipmaplevel;
        uint32_t mipmapsize;
    };
    //////////////////////////////////////////////////////////////////////////
    struct PickCodecOptions
        : public CodecOptions
    {
        size_t pitch;
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
}
