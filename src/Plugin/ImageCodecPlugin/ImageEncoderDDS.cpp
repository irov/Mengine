#include "ImageEncoderDDS.h"

#include "ImageCodecDDS.h"

#include "Kernel/PixelFormat.h"

#include "Kernel/Magic.h"
#include "Kernel/Logger.h"

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
	size_t ImageEncoderDDS::encode( const void * _buffer, size_t _size, const CodecDataInfo* _dataInfo )
	{   
		(void)_size;

		const ImageCodecDataInfo * imageInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

		uint32_t magic = FOURCC( 'D', 'D', 'S', ' ' );
		m_stream->write( &magic, sizeof(magic) );

		uint32_t ddsSize = Helper::getTextureMemorySize( imageInfo->width, imageInfo->height, imageInfo->channels, imageInfo->depth, imageInfo->format );

		DDS_HEADER header;
		memset( &header, 0, sizeof( header ) );

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
		header.ddspf.dwFourCC = s_convertFormatFourCC( imageInfo->format );

        m_stream->write( &header, sizeof(header) );
		
		m_stream->write( _buffer, ddsSize );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	
