#include "ImageDecoderPVRTC.h"
#include "Kernel/Logger.h"

#include "Kernel/PixelFormat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EPixelType
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

    static const uint32_t PVRTEX_MIPMAP = (1 << 8); // has mip map levels
    static const uint32_t PVRTEX_TWIDDLE = (1 << 9); // is twiddled
    static const uint32_t PVRTEX_BUMPMAP = (1 << 10); // has normals encoded for a bump map
    static const uint32_t PVRTEX_TILING = (1 << 11); // is bordered for tiled pvr
    static const uint32_t PVRTEX_CUBEMAP = (1 << 12); // is a cubemap/skybox
    static const uint32_t PVRTEX_FALSEMIPCOL = (1 << 13); //
    static const uint32_t PVRTEX_VOLUME = (1 << 14);
    static const uint32_t PVRTEX_PIXELTYPE = 0xff; // pixel type is always in the last 16bits of the flags
    static const uint32_t PVRTEX_IDENTIFIER = 0x21525650;  // the pvr identifier is the characters 'P','V','R'
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t PVRTEX_V1_HEADER_SIZE = 44; // old header size was 44 for identification purposes
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderPVRTC::ImageDecoderPVRTC()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderPVRTC::~ImageDecoderPVRTC()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderPVRTC::_decode( void * _buffer, size_t _bufferSize )
    {
        // TODO: investigate why sizeof(PVRTextureHeader) != 52
        m_stream->seek( 52 + m_header.metaDataSize );
        size_t read = m_stream->read( _buffer, _bufferSize );

        return read == _bufferSize ? read : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderPVRTC::_prepareData()
    {
        m_stream->seek( 0 );

        m_stream->read( &m_header, 52 );

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
        m_dataInfo.channels = 3;
        m_dataInfo.depth = 1;

        if( (m_header.flags & PVRTEX_MIPMAP) == PVRTEX_MIPMAP )
        {
            m_dataInfo.mipmaps = 1 + m_header.numMipmaps;
        }
        else
        {
            m_dataInfo.mipmaps = 1;
        }

        bool isValid = m_header.numFaces == 1  // supported only 1 face
            && m_header.numSurfaces == 1;  // supported only 1 surface

        return isValid;
    }
}
