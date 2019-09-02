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
        ImageCodecDataInfo() noexcept
            : mipmaps( 1 )
            , width( 0 )
            , height( 0 )
            , channels( 0 )
            , depth( 1 )
            , quality( 100 )
            , format( PF_UNKNOWN )
        {
        }

        uint32_t mipmaps;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        uint32_t depth;

        int32_t quality;

        EPixelFormat format;

        uint32_t getFullSize() const
        {
            uint32_t full_size = 0;

            for( uint32_t i = 0; i != mipmaps; ++i )
            {
                uint32_t s = this->getMipMapSize( i );

                full_size += s;
            }

            return full_size;
        }

        uint32_t getMipMapSize( uint32_t _level ) const
        {
            uint32_t mipmap_width = (width >> _level);
            uint32_t mipmap_height = (height >> _level);

            uint32_t mipmap_size = Helper::getTextureMemorySize( mipmap_width, mipmap_height, channels, depth, format );

            return mipmap_size;
        }

        uint32_t getSize() const
        {
            uint32_t size = Helper::getTextureMemorySize( width, height, channels, depth, format );

            return size;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    enum EImageDecoderOptions
    {
        DF_NONE = 0x00000000,
        DF_COUNT_ALPHA = 0x00000001,
        DF_READ_ALPHA_ONLY = 0x00000002,
        DF_QUALITY = 0x00000004,
        DF_WRITE_ALPHA_ONLY = 0x00000008,
        DF_NOT_ADD_ALPHA = 0x00000010,
        DF_PREMULTIPLY_ALPHA = 0x00010000,
    };
    //////////////////////////////////////////////////////////////////////////
    struct ImageCodecOptions
        : public CodecOptions
    {
        ImageCodecOptions() noexcept
            : flags( DF_NONE )
            , channels( 0 )
            , mipmap( 0 )
            , pitch( 0 )
        {
        }

        uint32_t flags;
        uint32_t channels;
        uint32_t mipmap;
        size_t pitch;

        bool hasFlag( uint32_t _flag ) const
        {
            return (flags & _flag) != 0;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    class ImageDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const ImageCodecDataInfo * getCodecDataInfo() const override = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImageDecoderInterface> ImageDecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ImageEncoderInterface
        : public EncoderInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImageEncoderInterface> ImageEncoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
