#   pragma once

#   define DDS_MAGIC			0x20534444

#   define DDSD_CAPS			0x00000001
#   define DDSD_HEIGHT			0x00000002
#   define DDSD_WIDTH			0x00000004
#   define DDSD_PITCH			0x00000008
#   define DDSD_PIXELFORMAT 	0x00001000
#   define DDSD_MIPMAPCOUNT	0x00020000
#   define DDSD_LINEARSIZE		0x00080000
#   define DDSD_DEPTH			0x00800000 

//
// DDPIXELFORMAT flags
//
#   define DDPF_ALPHAPIXELS        0x00000001l
#   define DDPF_FOURCC             0x00000004l        // Compressed formats
#   define DDPF_RGB                0x00000040l        // Uncompressed formats
#   define DDPF_ALPHA              0x00000002l
#   define DDPF_COMPRESSED         0x00000080l
#   define DDPF_LUMINANCE          0x00020000l
#   define DDPF_BUMPLUMINANCE      0x00040000l        // L,U,V
#   define DDPF_BUMPDUDV           0x00080000l        // U,V

#   define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))

namespace Menge
{
#pragma pack( push, 1 )
    struct DDS_PIXELFORMAT {
        uint32 dwSize;
        uint32 dwFlags;
        uint32 dwFourCC;
        uint32 dwRGBBitCount;
        uint32 dwRBitMask;
        uint32 dwGBitMask;
        uint32 dwBBitMask;
        uint32 dwABitMask;
    };

    typedef struct {
        uint32           dwSize;
        uint32           dwFlags;
        uint32           dwHeight;
        uint32           dwWidth;
        uint32           dwPitchOrLinearSize;
        uint32           dwDepth;
        uint32           dwMipMapCount;
        uint32           dwReserved1[11];
        DDS_PIXELFORMAT	 ddspf;
        uint32           dwSurfaceFlags;
        uint32           dwCubemapFlags;
        uint32           dwReserved2[3];
    } DDS_HEADER;
#pragma pack(pop)
}