#include "ImageDecoderPVRTC.h"

#include "Kernel/Logger.h"
#include "Kernel/PixelFormat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EPVRTCPixelType
    {
        PVRTC2RGB = 0,
        PVRTC2RGBA,
        PVRTC4RGB,
        PVRTC4RGBA,
        PVRTCII2,
        PVRTCII4,
        ETC1,
        DXT1,
        DXT2,
        DXT3,
        DXT4,
        DXT5,
        BC1,
        BC2,
        BC3,
        BC4,
        BC5,
        BC6,
        BC7,
        UYVY,
        YUY2,
        BW1bpp,
        R9G9B9E5,
        RGBG8888,
        GRGB8888,
        ETC2RGB,
        ETC2RGBA,
        ETC2RGBA1,
        EACR11U,
        EACR11S,
        EACRG11U,
        EACRG11S,
    };
    //////////////////////////////////////////////////////////////////////////

    // has mip map levels
    #define PVRTEX_MIPMAP (1 << 8) 
     // is twiddled
    #define PVRTEX_TWIDDLE (1 << 9)
    // has normals encoded for a bump map
    #define PVRTEX_BUMPMAP (1 << 10)
     // is bordered for tiled pvr
    #define PVRTEX_TILING (1 << 11)
     // is a cubemap/skybox
    #define PVRTEX_CUBEMAP (1 << 12)
    #define PVRTEX_FALSEMIPCOL (1 << 13)
    #define PVRTEX_VOLUME (1 << 14)
     // pixel type is always in the last 16bits of the flags
    #define PVRTEX_PIXELTYPE (0xff)
    // the pvr identifier is the characters 'P','V','R'
    #define PVRTEX_IDENTIFIER (0x21525650)
    //////////////////////////////////////////////////////////////////////////
    // old header size was 44 for identification purposes
    #define PVRTEX_V1_HEADER_SIZE (44)
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderPVRTC::ImageDecoderPVRTC()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderPVRTC::~ImageDecoderPVRTC()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderPVRTC::_decode( const DecoderData * _data )
    {
        void * buffer = _data->buffer;
        size_t size = _data->size;

        size_t read = m_stream->read( buffer, size );

        return read == size ? read : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderPVRTC::_prepareData()
    {
        m_stream->read( &m_header, 52 );

        MENGINE_ASSERTION_FATAL( m_header.numFaces == 1 );
        MENGINE_ASSERTION_FATAL( m_header.numSurfaces == 1 );

        switch( m_header.pixelFormat )
        {
        case PVRTC4RGB:
            {
                m_dataInfo.format = PF_PVRTC4_RGB;
            }break;
        case PVRTC4RGBA:
            {
                m_dataInfo.format = PF_PVRTC4_RGBA;
            }break;
        case ETC1:
            {
                m_dataInfo.format = PF_ETC1;
            }break;
        case DXT1:
            {
                m_dataInfo.format = PF_DXT1;
            }break;
        default:
            {
                return false; // not supported format
            }break;
        }

        m_dataInfo.width = m_header.width;
        m_dataInfo.height = m_header.height;

        if( (m_header.flags & PVRTEX_MIPMAP) == PVRTEX_MIPMAP )
        {
            m_dataInfo.mipmaps = 1 + m_header.numMipmaps;
        }
        else
        {
            m_dataInfo.mipmaps = 1;
        }

        m_stream->skip( m_header.metaDataSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
