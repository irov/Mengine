#pragma once

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t DDS_MAGIC = 0x20534444;
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t DDSD_CAPS = 0x00000001;
    static constexpr uint32_t DDSD_HEIGHT = 0x00000002;
    static constexpr uint32_t DDSD_WIDTH = 0x00000004;
    static constexpr uint32_t DDSD_PITCH = 0x00000008;
    static constexpr uint32_t DDSD_PIXELFORMAT = 0x00001000;
    static constexpr uint32_t DDSD_MIPMAPCOUNT = 0x00020000;
    static constexpr uint32_t DDSD_LINEARSIZE = 0x00080000;
    static constexpr uint32_t DDSD_DEPTH = 0x00800000;
    //////////////////////////////////////////////////////////////////////////
    //
    // DDPIXELFORMAT flags
    //
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t DDPF_ALPHAPIXELS = 0x00000001l;
    static constexpr uint32_t DDPF_FOURCC = 0x00000004l;        // Compressed formats
    static constexpr uint32_t DDPF_RGB = 0x00000040l;        // Uncompressed formats
    static constexpr uint32_t DDPF_ALPHA = 0x00000002l;
    static constexpr uint32_t DDPF_COMPRESSED = 0x00000080l;
    static constexpr uint32_t DDPF_LUMINANCE = 0x00020000l;
    static constexpr uint32_t DDPF_BUMPLUMINANCE = 0x00040000l;        // L,U,V
    static constexpr uint32_t DDPF_BUMPDUDV = 0x00080000l;        // U,V
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t DDSF_COMPLEX = 0x00000008;
    static constexpr uint32_t DDSF_TEXTURE = 0x00001000;
    static constexpr uint32_t DDSF_MIPMAP = 0x00400000;
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t DDCF_CUBEMAP = 0x00000200;
    static constexpr uint32_t DDCF_CUBEMAP_POSITIVEX = 0x00000400;
    static constexpr uint32_t DDCF_CUBEMAP_NEGATIVEX = 0x00000800;
    static constexpr uint32_t DDCF_CUBEMAP_POSITIVEY = 0x00001000;
    static constexpr uint32_t DDCF_CUBEMAP_NEGATIVEY = 0x00002000;
    static constexpr uint32_t DDCF_CUBEMAP_POSITIVEZ = 0x00004000;
    static constexpr uint32_t DDCF_CUBEMAP_NEGATIVEZ = 0x00008000;
    static constexpr uint32_t DDCF_VOLUME = 0x00200000;
    //////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
    struct DDS_PIXELFORMAT
    {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;
    };

    typedef struct
    {
        uint32_t           dwSize;
        uint32_t           dwFlags;
        uint32_t           dwHeight;
        uint32_t           dwWidth;
        uint32_t           dwPitchOrLinearSize;
        uint32_t           dwDepth;
        uint32_t           dwMipMapCount;
        uint32_t           dwReserved1[11];
        DDS_PIXELFORMAT    ddspf;
        uint32_t           dwSurfaceFlags;
        uint32_t           dwCubemapFlags;
        uint32_t           dwReserved2[3];
    } DDS_HEADER;
#pragma pack(pop)
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE EPixelFormat convertFourCCFormat( uint32_t fourcc )
        {
            // convert dxt pixel format
            switch( fourcc )
            {
            case MENGINE_FOURCC( 'D', 'X', 'T', '1' ):
                return PF_DXT1;
            case MENGINE_FOURCC( 'D', 'X', 'T', '2' ):
                return PF_DXT2;
            case MENGINE_FOURCC( 'D', 'X', 'T', '3' ):
                return PF_DXT3;
            case MENGINE_FOURCC( 'D', 'X', 'T', '4' ):
                return PF_DXT4;
            case MENGINE_FOURCC( 'D', 'X', 'T', '5' ):
                return PF_DXT5;
            default:
                break;
            }

            return PF_UNKNOWN;
        };
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t convertFormatFourCC( EPixelFormat _format )
        {
            // convert dxt pixel format
            switch( _format )
            {
            case PF_DXT1:
                return MENGINE_FOURCC( 'D', 'X', 'T', '1' );
            case PF_DXT2:
                return MENGINE_FOURCC( 'D', 'X', 'T', '2' );
            case PF_DXT3:
                return MENGINE_FOURCC( 'D', 'X', 'T', '3' );
            case PF_DXT4:
                return MENGINE_FOURCC( 'D', 'X', 'T', '4' );
            case PF_DXT5:
                return MENGINE_FOURCC( 'D', 'X', 'T', '5' );
            default:
                break;
            }

            return MENGINE_FOURCC( 'U', 'N', 'K', 'N' );
        };
        //////////////////////////////////////////////////////////////////////////
    }
}