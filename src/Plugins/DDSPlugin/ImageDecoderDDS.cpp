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

        MENGINE_ASSERTION_FATAL( magic == MENGINE_FOURCC( 'D', 'D', 'S', ' ' ), "invalid dds magic" );

        DDS_HEADER header;
        stream->read( &header, sizeof( header ) );

        //Check valid structure sizes
        MENGINE_ASSERTION_FATAL( header.dwSize == 124 || header.ddspf.dwSize == 32, "invalid dds file header" );

        if( (header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT )
        {
            m_dataInfo.mipmaps = 1 + header.dwMipMapCount;
        }
        else
        {
            m_dataInfo.mipmaps = 1;
        }

        MENGINE_ASSERTION_FATAL( (header.ddspf.dwFlags & DDPF_FOURCC) != 0, "dds file no compress" );

        m_dataInfo.width = header.dwWidth;
        m_dataInfo.height = header.dwHeight;

        m_dataInfo.format = Detail::convertFourCCFormat( header.ddspf.dwFourCC );

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
