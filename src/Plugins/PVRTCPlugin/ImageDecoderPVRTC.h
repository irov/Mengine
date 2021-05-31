#pragma once

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    class ImageDecoderPVRTC
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderPVRTC );

    public:
        ImageDecoderPVRTC();
        ~ImageDecoderPVRTC() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( const DecoderData * _data ) override;

    protected:
        struct PVRTextureHeader
        {
            uint32_t version;
            uint32_t flags;
            uint64_t pixelFormat;
            uint32_t colorSpace;
            uint32_t channelType;
            uint32_t height;
            uint32_t width;
            uint32_t depth;
            uint32_t numSurfaces;
            uint32_t numFaces;
            uint32_t numMipmaps;
            uint32_t metaDataSize;
        };

        PVRTextureHeader m_header = {};
    };
}