#include "ImageDecoderDDS.h"

#include "ImageCodecDDS.h"

#include "Kernel/PixelFormat.h"

#include "Kernel/Magic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderDDS::ImageDecoderDDS()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderDDS::~ImageDecoderDDS()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderDDS::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderDDS::_prepareData()
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        uint32_t magic;
        stream->read( &magic, sizeof( magic ) );

        if( magic != MENGINE_FOURCC( 'D', 'D', 'S', ' ' ) )
        {
            LOGGER_ERROR( "invalid dds magic" );

            return false;
        }

        DDS_HEADER header;
        stream->read( &header, sizeof( header ) );

        //Check valid structure sizes
        if( header.dwSize != 124 && header.ddspf.dwSize != 32 )
        {
            LOGGER_ERROR( "invalid dds file header" );

            return false;
        }

        //const bool _CAPS = (header.dwFlags & DDSD_CAPS);
        //const bool _HEIGHT = (header.dwFlags & DDSD_HEIGHT);
        //const bool _WIDTH = (header.dwFlags & DDSD_WIDTH);
        //const bool _PITCH = (header.dwFlags & DDSD_PITCH);
        //const bool _PIXELFORMAT = (header.dwFlags & DDSD_PIXELFORMAT);
        //const bool _MIPMAPCOUNT = (header.dwFlags & DDSD_MIPMAPCOUNT);
        //const bool _LINEARSIZE = (header.dwFlags & DDSD_LINEARSIZE);
        //const bool _DEPTH = (header.dwFlags & DDSD_DEPTH);

        //const bool _COMPLEX = (header.dwSurfaceFlags & DDSF_COMPLEX);
        //const bool _TEXTURE = (header.dwSurfaceFlags & DDSF_TEXTURE);
        //const bool _MIPMAP = (header.dwSurfaceFlags & DDSF_MIPMAP);

        //const bool _CUBEMAP = (header.dwCubemapFlags & DDCF_CUBEMAP);
        //const bool _CUBEMAP_POSITIVEX = (header.dwCubemapFlags & DDCF_CUBEMAP_POSITIVEX);
        //const bool _CUBEMAP_NEGATIVEX = (header.dwCubemapFlags & DDCF_CUBEMAP_NEGATIVEX);
        //const bool _CUBEMAP_POSITIVEY = (header.dwCubemapFlags & DDCF_CUBEMAP_POSITIVEY);
        //const bool _CUBEMAP_NEGATIVEY = (header.dwCubemapFlags & DDCF_CUBEMAP_NEGATIVEY);
        //const bool _CUBEMAP_POSITIVEZ = (header.dwCubemapFlags & DDCF_CUBEMAP_POSITIVEZ);
        //const bool _CUBEMAP_NEGATIVEZ = (header.dwCubemapFlags & DDCF_CUBEMAP_NEGATIVEZ);
        //const bool _VOLUME = (header.dwCubemapFlags & DDCF_VOLUME);

        //const bool _ALPHAPIXELS = (header.ddspf.dwFlags & DDPF_ALPHAPIXELS);
        //const bool _FOURCC = (header.ddspf.dwFlags & DDPF_FOURCC);
        //const bool _RGB = (header.ddspf.dwFlags & DDPF_RGB);
        //const bool s_ALPHA = (header.ddspf.dwFlags & DDPF_ALPHA);
        //const bool _COMPRESSED = (header.ddspf.dwFlags & DDPF_COMPRESSED);
        //const bool _LUMINANCE = (header.ddspf.dwFlags & DDPF_LUMINANCE);
        //const bool _BUMPLUMINANCE = (header.ddspf.dwFlags & DDPF_BUMPLUMINANCE);
        //const bool _BUMPDUDV = (header.ddspf.dwFlags & DDPF_BUMPDUDV);

        if( (header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT )
        {
            m_dataInfo.mipmaps = 1 + header.dwMipMapCount;
        }
        else
        {
            m_dataInfo.mipmaps = 1;
        }

        if( (header.ddspf.dwFlags & DDPF_FOURCC) == 0 )
        {
            LOGGER_ERROR( "dds file no compress"
            );

            return false;
        }

        m_dataInfo.width = header.dwWidth;
        m_dataInfo.height = header.dwHeight;

        m_dataInfo.format = s_convertFourCCFormat( header.ddspf.dwFourCC );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderDDS::_decode( const DecoderData * _data )
    {
        void * buffer = _data->buffer;
        size_t size = _data->size;

        const InputStreamInterfacePtr & stream = this->getStream();

        size_t byte = stream->read( buffer, size );

        return byte;
    }
    //////////////////////////////////////////////////////////////////////////
}
