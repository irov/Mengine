#include "ImageEncoderDDS.h"

#include "ImageCodecDDS.h"

#include "Kernel/PixelFormat.h"
#include "Kernel/Magic.h"
#include "Kernel/Logger.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageEncoderDDS::ImageEncoderDDS()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageEncoderDDS::~ImageEncoderDDS()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoderDDS::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderDDS::encode( const void * _buffer, size_t _size, const CodecDataInfo * _dataInfo )
    {
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const ImageCodecDataInfo * );

        const ImageCodecDataInfo * imageInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

        size_t write_size = 0;

        uint32_t magic = MENGINE_FOURCC( 'D', 'D', 'S', ' ' );
        write_size += m_stream->write( &magic, sizeof( magic ) );

        uint32_t ddsSize = Helper::getTextureMemorySize( imageInfo->width, imageInfo->height, imageInfo->channels, imageInfo->depth, imageInfo->format );

        DDS_HEADER header;
        MENGINE_MEMSET( &header, 0, sizeof( header ) );

        header.dwSize = 124;
        header.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
        header.dwHeight = imageInfo->height;
        header.dwWidth = imageInfo->width;
        header.dwPitchOrLinearSize = ddsSize;
        header.dwDepth = 0;
        header.dwMipMapCount = 0;
        header.dwSurfaceFlags = DDSF_TEXTURE;
        header.dwCubemapFlags = 0;

        header.ddspf.dwSize = 32;
        header.ddspf.dwFlags = DDPF_FOURCC;
        header.ddspf.dwFourCC = Detail::convertFormatFourCC( imageInfo->format );

        write_size += m_stream->write( &header, sizeof( header ) );
        write_size += m_stream->write( _buffer, ddsSize );

        return write_size;
    }
    //////////////////////////////////////////////////////////////////////////
}
