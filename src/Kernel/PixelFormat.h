#pragma once

#include "Config/Config.h"

namespace Mengine
{
    enum EPixelFormat
    {
        PF_UNKNOWN = 0,

        PF_L8 = 1,
        PF_BYTE_L = PF_L8,

        PF_L16 = 2,
        PF_SHORT_L = PF_L16,

        PF_A8 = 3,
        PF_BYTE_A = PF_A8,

        PF_A4L4 = 4,
        PF_BYTE_LA = 5,
        PF_R5G6B5 = 6,
        PF_B5G6R5 = 7,
        PF_R3G3B2 = 31,
        PF_A4R4G4B4 = 8,
        PF_A1R5G5B5 = 9,
        PF_R8G8B8 = 10,
        PF_B8G8R8 = 11,
        PF_A8R8G8B8 = 12,
        PF_A8B8G8R8 = 13,
        PF_B8G8R8A8 = 14,
        PF_R8G8B8A8 = 28,
        PF_X8R8G8B8 = 26,
        PF_X8B8G8R8 = 27,

        PF_A2R10G10B10 = 15,
        PF_A2B10G10R10 = 16,
        PF_DXT1 = 17,
        PF_DXT2 = 18,
        PF_DXT3 = 19,
        PF_DXT4 = 20,
        PF_DXT5 = 21,
        PF_FLOAT16_R = 32,
        PF_FLOAT16_RGB = 22,
        PF_FLOAT16_RGBA = 23,
        PF_FLOAT32_R = 33,
        PF_FLOAT32_RGB = 24,
        PF_FLOAT32_RGBA = 25,
        PF_FLOAT16_GR = 35,
        PF_FLOAT32_GR = 36,
        PF_DEPTH = 29,
        PF_SHORT_RGBA = 30,
        PF_SHORT_GR = 34,
        PF_SHORT_RGB = 37,

        PF_ETC1 = 38,
        PF_PVRTC4_RGB = 39,
        PF_PVRTC4_RGBA = 40,

        PF_COUNT = 41
    };
}