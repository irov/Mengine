#pragma once

#include "Interface/DecoderInterface.h"
#include "Interface/EncoderInterface.h"

#include "Kernel/PixelFormat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ImageCodecDataInfo
        : public CodecDataInfo
    {
        uint32_t mipmaps = 1;
        uint32_t width = 0;
        uint32_t height = 0;

        int32_t quality = 100;

        EPixelFormat format = PF_UNKNOWN;
    };
    //////////////////////////////////////////////////////////////////////////
    enum EImageDecoderFlags
    {
        DF_IMAGE_NONE = 0x00000000,
        DF_IMAGE_COUNT_ALPHA = 0x00000001,
        DF_IMAGE_READ_ALPHA_ONLY = 0x00000002,
        DF_IMAGE_QUALITY = 0x00000004,
        DF_IMAGE_WRITE_ALPHA_ONLY = 0x00000008,
        DF_IMAGE_NOT_ADD_ALPHA = 0x00000010,

        DF_IMAGE_PREMULTIPLY_ALPHA = 0x00010000,
    };
    //////////////////////////////////////////////////////////////////////////
    struct ImageDecoderData
        : public DecoderData
    {
        size_t pitch;
        EPixelFormat format;

        uint32_t flags = DF_IMAGE_NONE;
        uint32_t mipmap = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct ImageEncoderData
        : public EncoderData
    {
        size_t pitch;
    };
    //////////////////////////////////////////////////////////////////////////
    class ImageDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const ImageCodecDataInfo * getCodecDataInfo() const override = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImageDecoderInterface, DecoderInterface> ImageDecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ImageEncoderInterface
        : public EncoderInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImageEncoderInterface, EncoderInterface> ImageEncoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
