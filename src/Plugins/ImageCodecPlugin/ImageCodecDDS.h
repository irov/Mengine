#pragma once

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t DDS_MAGIC = 0x20534444;
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t DDSD_CAPS = 0x00000001;
    static const uint32_t DDSD_HEIGHT = 0x00000002;
    static const uint32_t DDSD_WIDTH = 0x00000004;
    static const uint32_t DDSD_PITCH = 0x00000008;
    static const uint32_t DDSD_PIXELFORMAT = 0x00001000;
    static const uint32_t DDSD_MIPMAPCOUNT = 0x00020000;
    static const uint32_t DDSD_LINEARSIZE = 0x00080000;
    static const uint32_t DDSD_DEPTH = 0x00800000;
    //////////////////////////////////////////////////////////////////////////
    //
    // DDPIXELFORMAT flags
    //
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t DDPF_ALPHAPIXELS = 0x00000001l;
    static const uint32_t DDPF_FOURCC = 0x00000004l;        // Compressed formats
    static const uint32_t DDPF_RGB = 0x00000040l;        // Uncompressed formats
    static const uint32_t DDPF_ALPHA = 0x00000002l;
    static const uint32_t DDPF_COMPRESSED = 0x00000080l;
    static const uint32_t DDPF_LUMINANCE = 0x00020000l;
    static const uint32_t DDPF_BUMPLUMINANCE = 0x00040000l;        // L,U,V
    static const uint32_t DDPF_BUMPDUDV = 0x00080000l;        // U,V
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t DDSF_COMPLEX = 0x00000008;
    static const uint32_t DDSF_TEXTURE = 0x00001000;
    static const uint32_t DDSF_MIPMAP = 0x00400000;
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t DDCF_CUBEMAP = 0x00000200;
    static const uint32_t DDCF_CUBEMAP_POSITIVEX = 0x00000400;
    static const uint32_t DDCF_CUBEMAP_NEGATIVEX = 0x00000800;
    static const uint32_t DDCF_CUBEMAP_POSITIVEY = 0x00001000;
    static const uint32_t DDCF_CUBEMAP_NEGATIVEY = 0x00002000;
    static const uint32_t DDCF_CUBEMAP_POSITIVEZ = 0x00004000;
    static const uint32_t DDCF_CUBEMAP_NEGATIVEZ = 0x00008000;
    static const uint32_t DDCF_VOLUME = 0x00200000;
    //////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
    struct DDS_PIXELFORMAT {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;
    };

    typedef struct {
        uint32_t           dwSize;
        uint32_t           dwFlags;
        uint32_t           dwHeight;
        uint32_t           dwWidth;
        uint32_t           dwPitchOrLinearSize;
        uint32_t           dwDepth;
        uint32_t           dwMipMapCount;
        uint32_t           dwReserved1[11];
        DDS_PIXELFORMAT	 ddspf;
        uint32_t           dwSurfaceFlags;
        uint32_t           dwCubemapFlags;
        uint32_t           dwReserved2[3];
    } DDS_HEADER;
#pragma pack(pop)
    //////////////////////////////////////////////////////////////////////////
#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE EPixelFormat s_convertFourCCFormat( uint32_t fourcc )
    {
        // convert dxt pixel format
        switch( fourcc )
        {
        case FOURCC( 'D', 'X', 'T', '1' ):
            return PF_DXT1;
        case FOURCC( 'D', 'X', 'T', '2' ):
            return PF_DXT2;
        case FOURCC( 'D', 'X', 'T', '3' ):
            return PF_DXT3;
        case FOURCC( 'D', 'X', 'T', '4' ):
            return PF_DXT4;
        case FOURCC( 'D', 'X', 'T', '5' ):
            return PF_DXT5;
        }

        return PF_UNKNOWN;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t s_convertFormatFourCC( EPixelFormat _format )
    {
        // convert dxt pixel format
        switch( _format )
        {
        case PF_DXT1:
            return FOURCC( 'D', 'X', 'T', '1' );
        case PF_DXT2:
            return FOURCC( 'D', 'X', 'T', '2' );
        case PF_DXT3:
            return FOURCC( 'D', 'X', 'T', '3' );
        case PF_DXT4:
            return FOURCC( 'D', 'X', 'T', '4' );
        case PF_DXT5:
            return FOURCC( 'D', 'X', 'T', '5' );
        default:
            break;
        }

        return FOURCC( 'U', 'N', 'K', 'N' );
    };
}
